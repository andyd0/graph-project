#include <climits>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <omp.h>

using namespace std;

void swap(char *a, char *b);
void permutations(char *input, int l, int r, int n, int &j, vector<vector<int> > &permute_list);
int factorial(int n);
void shortest_list(vector<vector<int> > permute_list, int **cities, int threads);
int check_graph(vector<int> path, int **cities, int min);
vector<int> ithPermutation(const int n, int i);

int main(int argc, char *argv[]) {

	if (argc < 4) {
		printf("Please use the right number of arguments\n");
		exit(0);
	}

	int n = atoi(argv[1]);
	int threads = atoi(argv[2]);

	int **cities = new int*[n];
	for (int i = 0; i < n; i++)
		cities[i] = new int[n];

	string line;

	ifstream inputFile(argv[3]);

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			inputFile >> cities[i][j];
		}
	}

	int fact = factorial(n);
	vector<vector<int> > permute_list(fact, vector<int>(n - 1, 0));

	char *numbers = new char[n];
	for (int i = 0; i < (n - 1); i++) {
		numbers[i] = (i + 1) + '0';
	}
	numbers[n - 1] = 0;

	omp_set_num_threads(threads);
	# pragma omp parallel for
	for(int i = 0; i < fact; i++) {
		permute_list.at(i) = ithPermutation(n , i);
	}

	shortest_list(permute_list, cities, threads);
}


void swap(char *a, char *b) {
	char temp;
	temp = *a;
	*a = *b;
	*b = temp;
}


int factorial(int n) {
	int value = 1;
	for (int i = n - 1; i > 1; i--)
		value *= i;
	return value;
}


vector<int> ithPermutation(const int n, int i)
{
   int j, k = 0;
   vector<int> factorials(n);
   vector<int> permutation(n);

   // compute factorial numbers
   factorials.at(k) = 1;
   while (++k < n)
      factorials.at(k) = factorials.at(k - 1) * k;

   // compute factorial code
   for (k = 1; k < n; ++k)
   {
      permutation.at(k) = i / factorials.at(n - 1 - k);
      i = i % factorials.at(n - 1 - k);
   }

   // readjust values to obtain the permutation
   // start from the end and check if preceding values are lower
   for (k = n - 1; k > 0; --k)
      for (j = k - 1; j >= 0; --j)
         if (permutation.at(j) <= permutation.at(k))
            permutation.at(k)++;
	
   	return permutation;
}


void shortest_list(vector<vector<int> > permute_list, int **cities, int threads) {

	vector<int> shortest_path;
	int cost = INT_MAX;
	int n = permute_list.size();
	int i;

	omp_set_num_threads(threads);
	# pragma omp parallel for
	for (i = 0; i < n; i++) {
		int temp = check_graph(permute_list.at(i), cities, cost);
		if (cost > temp) {
			cost = temp;
			shortest_path = permute_list.at(i);
		}
	}

	cout << "Shortest path is:" << endl;
	for (int x : shortest_path)
		printf("%d ", x);
	printf("\ntotal weight = %d\n", cost);
}

int check_graph(vector<int> path, int **cities, int cost) {

	int path_sum = 0;
	int path_size = path.size();
	int i = 0;
	int j = 0;
	int x = 0;
	int y = 1;

	while (y < path_size) {
		i = path[x];
		j = path[y];
		path_sum += cities[i][j];
		if (path_sum > cost) {
			path_sum = INT_MAX;
			break;
		}
		x++;
		y++;
	}

	return path_sum;
}
