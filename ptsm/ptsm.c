#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// Gets length of array based on type of array
#define LENGTH(x)  (sizeof(x) / sizeof((x)[0]))

int factorial(int n);

int main(int argc, char *argv[]) {

	int n = atoi(argv[1]);
	int threads = atoi(argv[2]);

	int **cities = (int **)malloc(n * sizeof(int *));
	for (int i = 0; i < n; i++)
		cities[i] = (int *)malloc(n * sizeof(int));

	FILE *file;
	file = fopen(argv[3], "r");
	if (file == NULL) exit(1);

	// Filling in matrix from input
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
			fscanf(file, "%d", &cities[i][j]);
	}

	// Building a path initially in ascending order.  
	// Will check the path again but cost insignificant
	char *initial = (char *)malloc(n * sizeof(char));
	for (int i = 1; i < n; i++)
		initial[i - 1] = i + '0';
	initial[n - 1] = '\0';

	int length = strlen(initial);
	int cost = 0;

	// Gets the cost of the initial path to have something
	// to work with when checking for path length.
	for (int i = 0; i < length; i++) {
		int j = i + 1;
		cost += cities[i][j];
	}

	// Will be used to keep track of cost and min path
	char * min_path = malloc(strlen(initial) + 1);
	strcpy(min_path, initial);

	// Number of permutations expected not including 0
	int fact = factorial(length);

	// The following code builds a permutation based on the
	// number of cities visited but not including 0.  As it builds
	// a permutation digit by digit, the path cost is checked.  If
	// it is min, cost and path are updated after the inner loop.
	// Number of threads used are based on value given when invoking
	// app
	omp_set_num_threads(threads);
	# pragma omp parallel for		
	for (int i = 0; i < fact; i++)
	{
		char * avail = malloc(strlen(initial) + 1);
		strcpy(avail, initial);
		char * path_build = malloc(strlen(initial) + 1);
		path_build[n - 1] = '\0';
		int prev = 0;
		int path_cost = 0;
		int path_length = 0;

		for (int j = length, divisor = fact; j > 0; j--)
		{
			divisor /= j;
			int index = (i / divisor) % j;
			path_cost += cities[prev][avail[index] - '0'];

			// Checking to see if the current build of the 
			// path and its cost is greater than what the min cost
			// is so far.  Break if it is.  Path length will be shorter
			// than expected length so will not cause any issues.
			if (path_cost > cost) {
				break;
			}
			else {
				path_build[path_length] = avail[index];
				path_length++;
				prev = avail[index] - '0';
			}

			avail[index] = avail[j - 1];
		}

		// Checks to see if the path is the required
		// length.  If it is, that means we have found a minimum
		// path.  Since this is done in parallel, path may change
		// but still a legitimate path.
		if (path_length == length) {
			min_path = path_build;
			cost = path_cost;
		}
	}


	// Prints out results
	printf("%s", "Best path is: 0 ");
	for(int i = 0; i < length; i++)
		printf("%c ", min_path[i]);
	printf("\nDistance: %d\n", cost);

	free(initial);
	free(cities);
	free(min_path);
}

// Gets the factorial value based on n
int factorial(int n) {
	int value = 1;
	for (int i = n; i > 1; i--)
		value *= i;
	return value;
}