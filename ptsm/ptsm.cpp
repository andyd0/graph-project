#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <omp.h>

using namespace std;

int factorial(int n);

int main(int argc, char *argv[]) {

	int n = atoi(argv[1]);
	int threads = atoi(argv[2]);

	int **cities = new int*[n];
	for (int i = 0; i < n; i++)
		cities[i] = new int[n];

	ifstream inputFile(argv[3]);

	// Filling in matrix from input
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			inputFile >> cities[i][j];
		}
	}

	// Building a path initially in ascending order.  
	// Will check the path again but cost insignificant
	std::string initial = "";
	for (int i = 1; i < n; i++)
		initial += i + '0';

	int length = initial.size();
	int cost = 0;

	// Gets the cost of the initial path to have something
	// to work with when checking for path length.
	for (int i = 0; i < length; i++) {
		int j = i + 1;
		cost += cities[i][j];
	}

	// Will be used to keep track of cost and min path
	std::string min_path = initial;

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
		std::string avail = initial;
		std::string path_build = "";
		int prev = 0;
		int path_cost = 0;

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
				path_build += avail[index];
				prev = avail[index] - '0';
			}

			avail[index] = avail[j - 1];
		}

		// Checks to see if the path is the required
		// length.  If it is, that means we have found a minimum
		// path.  Since this is done in parallel, path may change
		// but still a legitimate path.
		if (path_build.size() == min_path.size()) {
			min_path = path_build;
			cost = path_cost;
		}
	}

	cout << "\nBest path is: ";
	cout << "0 ";
	for (char c : min_path)
		cout << c << " ";
	cout << endl;
	cout << "Distance: " << cost << endl;
}

// Gets the factorial value based on n
int factorial(int n) {
	int value = 1;
	for (int i = n; i > 1; i--)
		value *= i;
	return value;
}