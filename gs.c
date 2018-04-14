#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>


/***** Globals ******/
float *a; /* The coefficients */
float *x;  /* The unknowns */
float *b;  /* The constants */
float err; /* The absolute relative error */
int nit = 0; /* number of iterations */
int num = 0;  /* number of unknowns */


/****** Function declarations ******/
void get_input();  /* Read input from file */
void find_solution(); /* Does matrix vector multiplication to find solution */


/******************************************************/
/* Read input from file */
/* Function will have initialized the following:
* a[] will be filled with coefficients - a flattened n*n matrix
* x[] will contain the initial values of x
* b[] will contain the constants
* n will have nber of variables
* err will have the absolute error that you need to reach
*/
void get_input(FILE* fp, int process_rank, int comm_size, 
			   int local_num, float* local_a, float* local_b)
{
	if(process_rank == 0)
	{
		// Allocating for a 1D array as the matrix will be flattened
		// to a n*n array
		a = (float *)malloc(num * num * sizeof(float));
		if (!a)
		{
			printf("Cannot allocate a!\n");
			exit(1);
		}

		/* Now, time to allocate the matrices and vectors */
		x = (float *)malloc(num * sizeof(float));
		if (!x)
		{
			printf("Cannot allocate x!\n");
			exit(1);
		}

		b = (float *)malloc(num * sizeof(float));
		if (!b)
		{
			printf("Cannot allocate b!\n");
			exit(1);
		}

		/* The initial values of Xs */
		for(int i = 0; i < num; i++)
			fscanf(fp, "%f ", &x[i]);

		// Filling the matrix into a n*n array so that it can be partitioned. 
		// Also filling in the b array
		for (int i = 0; i < num; i++)
		{
			for (int j = 0; j < num; j++) {
				fscanf(fp, "%f ", &a[i * num + j]);
			}

			/* reading the b element */
			fscanf(fp, "%f ", &b[i]);
		}

		// Scatter the rows of the matrix A, elements of vector x, 
		// and elements of the vector b by blocks to available processes
		MPI_Scatter(a, num * local_num, MPI_FLOAT,
				    local_a, num * local_num, MPI_FLOAT,
					0, MPI_COMM_WORLD);

		// Scatter the matching elements of x
		MPI_Scatter(b, local_num, MPI_FLOAT,
				    local_b, local_num, MPI_FLOAT,
					0, MPI_COMM_WORLD);

		free(a);
		free(b);
		fclose(fp);
	}
	else
	{ 
		// Scatter the rows of the matrix A, elements of vector x, 
		// and elements of the vector b by blocks to available processes
		MPI_Scatter(a, num * local_num, MPI_FLOAT,
				    local_a, num * local_num, MPI_FLOAT,
					0, MPI_COMM_WORLD);

		// Scatter the matching elements of x
		MPI_Scatter(b, local_num, MPI_FLOAT,
				    local_b, local_num, MPI_FLOAT,
					0, MPI_COMM_WORLD);
	}
}

void find_solution(int process_rank, int comm_size, int local_num,
				   float* local_a, float* local_x, float* local_b)
{
	int passed = 0;
	float *new_x = (float *)malloc(num * sizeof(float));

	while (passed < 1)
	{
		passed = 0;

		// printf("local_num:\n");
		// printf("%d\n", local_num);
		// printf("process_rank:\n");
		// printf("%d\n", process_rank);
		// printf("comm size:\n");
		// printf("%d\n", comm_size);

		for (int i = 0; i < local_num; i++)
		{
			int global_i = i + (process_rank * local_num);

			// Avoiding the diagonals with two loops without
			// the use of an if check
			// Checks bottom triangle of matrix
			for (int j = 0; j < global_i; j++)
				local_x[i] -= (local_a[(i * num) + global_i] * x[j]);

			// And then checks top triangle of matrix
			for (int j = global_i + 1; j < num; j++)
				local_x[i] -= (local_a[(i * num) + global_i] * x[j]);

			local_x[i] = (local_b[i] - local_x[i]) / local_a[(i * num) + global_i];
		}

		if(process_rank == 1) {
			for(int i = 0; i < local_num; i++)
				printf("%f\n", local_x[i]);
			for(int i = 0; i < num; i++)
				printf("%f\n", x[i]);
		}

		// MPI_Allgather(local_x, local_num, MPI_FLOAT, new_x, local_num,
		// 			  MPI_FLOAT, MPI_COMM_WORLD);

		// Checking to see if error is below the threshold for all
		// the new values of x.
		// for (int i = 0; i < num; i++)
		// {
		// 	if (fabs((new_x[i] - x[i]) / new_x[i]) <= err) passed++;
		// }

		float* temp = x;
		x = new_x;
		new_x = temp;

		nit++;
		passed++;
	}
}

int main(int argc, char *argv[])
{
	FILE* fp;
	FILE* fp_out;
	char output[100] = "";



	if (argc != 2)
	{
		printf("Usage: ./gsc filename\n");
		exit(1);
	}

	fp = fopen(argv[1], "r");
	if (!fp)
	{
		printf("Cannot open file %s\n", argv[1]);
		exit(1);
	}

	// Reading in the number of unknowns and the error
	// rate
	fscanf(fp, "%d ", &num);
	fscanf(fp, "%f ", &err);

	int comm_size;
	int process_rank;
	int local_num;

	// Used for partitioning the inputs for each 
	// process
	float* local_a = (float *)malloc(num * num * sizeof(float));
	float* local_b = (float *)malloc(num * sizeof(float));

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

	MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&err, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

	local_num = num / comm_size;

	/* Read the input file and fill the global data structure above */
	get_input(fp, process_rank, comm_size, local_num, local_a, local_b);

	if(process_rank != 0) {
		x = (float *)malloc(num * sizeof(float));
	}

	MPI_Bcast(x, num, MPI_INT, 0, MPI_COMM_WORLD);

	float* local_x = (float *)malloc(num * sizeof(float));
	if(process_rank == 1) {
		for(int i = 0; i < local_num; i++)
			printf("%f\n", local_x[i]);
	}

	find_solution(process_rank, comm_size, local_num, local_a, local_x, local_b);

	// if(process_rank == 1) {
	// 	for(int i = 0; i < num; i++)
	// 		printf("%f\n", local_b[i]);
	// }

	free(local_a);
	free(local_b);
	free(local_x);
	free(x);
	MPI_Finalize();

	/* Writing results to file */
	sprintf(output, "%d.sol", num);
	fp_out = fopen(output, "w");
	// if (!fp)
	// {
	// 	printf("Cannot create the file %s\n", output);
	// 	exit(1);
	// }
	// DO I FUCKING NEED THIS???? WHY CANT I STOP THIS SHIT
	// if(process_rank == 0) {
	// 	printf("x computed values:\n");
	// 	for (int i = 0; i < num; i++)
	// 		printf("%f\n", x[i]);
	// }

	// printf("total nber of iterations: %d\n", nit);

	fclose(fp_out);

	return(0);
}
