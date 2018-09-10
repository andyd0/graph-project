#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/***** Globals ******/
float **a; /* The coefficients */
float *x;  /* The unknowns */
float *b;  /* The constants */
float err; /* The absolute relative error */
int nit = 0; /* number of iterations */
int num = 0;  /* number of unknowns */


/****** Function declarations ******/
void check_matrix(); /* Check whether the matrix will converge */
void get_input();  /* Read input from file */
void find_solution(); /* Does matrix vector multiplication to find solution */

/*
Checks to see if the given matrix will (diagonal dominance):
1. diagonal element >= sum of all other elements of the row
2. At least one diagonal element > sum of all other elements of the row
*/
void check_matrix()
{
	int bigger = 0; /* Set to 1 if at least one diag element > sum  */
	int i, j;
	float sum = 0;
	float aii = 0;

	for (i = 0; i < num; i++)
	{
		sum = 0;
		aii = fabs(a[i][i]);

		for (j = 0; j < num; j++)
			if (j != i)
				sum += fabs(a[i][j]);

		if (aii < sum)
		{
			printf("The matrix will not converge.\n");
			exit(1);
		}

		if (aii > sum)
			bigger++;

	}

	if (!bigger)
	{
		printf("The matrix will not converge\n");
		exit(1);
	}
}


/******************************************************/
/* Read input from file
* Function will have initialized the following:
* a[][] matrix will be filled with coefficients
* x[] will contain the initial values of x
* b[] will contain the constants
* num will have number of variables
* err will have the absolute error that you need to reach
*/
void get_input(char filename[])
{
	FILE * fp;
	int i, j;

	fp = fopen(filename, "r");
	if (!fp)
	{
		printf("Cannot open file %s\n", filename);
		exit(1);
	}

	fscanf(fp, "%d ", &num);
	fscanf(fp, "%f ", &err);

	/* Now, time to allocate the matrices and vectors */
	a = (float**)malloc(num * sizeof(float*));
	if (!a)
	{
		printf("Cannot allocate a!\n");
		exit(1);
	}

	for (i = 0; i < num; i++)
	{
		a[i] = (float *)malloc(num * sizeof(float));
		if (!a[i])
		{
			printf("Cannot allocate a[%d]!\n", i);
			exit(1);
		}
	}

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

	/* Now .. Filling the blanks */

	/* The initial values of Xs */
	for (i = 0; i < num; i++)
		fscanf(fp, "%f ", &x[i]);

	for (i = 0; i < num; i++)
	{
		for (j = 0; j < num; j++)
			fscanf(fp, "%f ", &a[i][j]);

		/* reading the b element */
		fscanf(fp, "%f ", &b[i]);
	}

	fclose(fp);
}

void find_solution()
{
	int passed = 0;
	float *new_x = (float *)malloc(num * sizeof(float));

	while (passed < num)
	{
		passed = 0;
		for (int i = 0; i < num; i++)
		{
			float temp = 0.0;

			// Avoiding the diagonals with two loops without
			// the use of an if check
			// Checks bottom triangle of matrix
			for (int j = 0; j < i; j++)
				temp += (a[i][j] * x[j]);
			// Checks top triangle of matrix
			for (int j = i + 1; j < num; j++)
				temp += (a[i][j] * x[j]);

			new_x[i] = (b[i] - temp) / a[i][i];
		}
		for (int i = 0; i < num; i++)
		{
			if (fabs((new_x[i] - x[i]) / new_x[i]) <= err) passed++;
			x[i] = new_x[i];
		}
		nit++;
	}
}

int main(int argc, char *argv[])
{
	int i;
	FILE * fp;
	char output[100] = "";

	if (argc != 2)
	{
		printf("Usage: ./gsc filename\n");
		exit(1);
	}

	/* Read the input file and fill the global data structure above */
	get_input(argv[1]);

	/* Check for convergence condition */
	/* This function will exit the program if the coffeicient will never converge to
	* the needed absolute error.
	*/
	check_matrix();

	find_solution();

	/* Writing results to file */
	sprintf(output, "%d.sol", num);
	fp = fopen(output, "w");
	if (!fp)
	{
		printf("Cannot create the file %s\n", output);
		exit(1);
	}

	for (i = 0; i < num; i++)
		fprintf(fp, "%f\n", x[i]);

	printf("total number of iterations: %d\n", nit);

	fclose(fp);

	exit(0);
}
