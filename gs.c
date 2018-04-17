#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
void get_sendcounts(); /* Splits up local_x for each processor to handle uneven splits */
void get_input();  /* Read input from file */
void find_solution(); /* Does matrix vector multiplication to find solution */


void get_sendcounts(int process_rank, int num, int comm_size, int* send_counts, int* displs)
{
	// calculate send counts and displacements
    for (int i = 0; i < comm_size; i++) {
        send_counts[i] = num / comm_size;
    }

	int remainder = num % comm_size;

	while(remainder > 0)
	{
		for(int i = 0; i < comm_size; i++) 
		{			
			remainder--;
			if(remainder < 0) break;
			send_counts[i]++;
		}
	}

	displs[0] = 0;

	for(int i = 1; i < comm_size; i++)
		displs[i] = displs[i - 1] + send_counts[i - 1];
}


/******************************************************/
/* Read input from file */
/* Function will have initialized the following:
* a[] will be filled with coefficients - a flattened n*n matrix
* x[] will contain the initial values of x
* b[] will contain the constants
* n will have nber of variables
* err will have the absolute error that you need to reach
*/
void get_input(char filename[])
{
	FILE * fp;
	fp = fopen(filename, "r");
	if (!fp)
	{
		printf("Cannot open file %s\n", filename);
		exit(1);
	}
	
	// Reading in the number of unknowns and the error
	// rate
	fscanf(fp, "%d ", &num);
	fscanf(fp, "%f ", &err);

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
}

void find_solution(int process_rank, int comm_size, int* send_counts, int* displacements)
{
	int passed = 0;
	float *new_x = (float *)malloc(num * sizeof(float));
	float* local_x = (float *)malloc(send_counts[process_rank] * sizeof(float));

	while (passed < num)
	{
		passed = 0;

		for (int i = 0; i < send_counts[process_rank]; i++)
		{
			int global_i = i + displacements[process_rank];
			local_x[i] = b[global_i];

			// Avoiding the diagonal with two loops without
			// the use of an if check

			// Checks bottom triangle of matrix
			for (int j = 0; j < global_i; j++) { 
				local_x[i] -= (a[(global_i * num) + j] * x[j]);
 			}

			// Checks top triangle of matrix
			for (int j = global_i + 1; j < num; j++) { 
				local_x[i] -= (a[(global_i * num) + j] * x[j]);
			}
			
			// Final calculation of local_x
			local_x[i] = local_x[i]/a[(global_i * num) + global_i];
		}

		// Gathers the local_x values from each process into new_rank
		// which is of size num
		MPI_Allgatherv(local_x, send_counts[process_rank], MPI_FLOAT,
				       new_x, send_counts, displacements, MPI_FLOAT, MPI_COMM_WORLD);


		// Checking to see if error is below the threshold for all
		// the new values of x.
		for (int i = 0; i < num; i++)
		{
			if (fabs((new_x[i] - x[i]) / new_x[i]) <= err) passed++;
		}

		// Swapping old and new_x for next iteration using pointers
		// instead of iteration
		float* temp = x;
		x = new_x;
		new_x = temp;

		nit++;
	}
}

int main(int argc, char *argv[])
{
	FILE* fp;
	char output[100] = "";

	if (argc != 2)
	{
		printf("Usage: ./gsc filename\n");
		exit(1);
	}

	int comm_size;
	int process_rank;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

	/* Read the input file and fill the global data structure above */
	get_input(argv[1]);

	int* send_counts = (int *)malloc(num * sizeof(int));
	int* displacements = (int *)malloc(num * sizeof(int));
	get_sendcounts(process_rank, num, comm_size, send_counts, displacements);

	find_solution(process_rank, comm_size, send_counts, displacements);

	/* Writing results to file */
	if(process_rank == 0) {
		sprintf(output, "%d.sol", num);
		fp = fopen(output, "w");
		if (!fp)
		{
			printf("Cannot create the file %s\n", output);
			exit(1);
		}

		 for(int i = 0; i < num; i++)
   			fprintf(fp,"%f\n",x[i]);

		printf("Total number of iterations: %d\n", nit);

		fclose(fp);
	}	

	free(a);
	free(b);
	free(x);
	free(send_counts);
	free(displacements);
	
	MPI_Finalize();

	return(0);
}
