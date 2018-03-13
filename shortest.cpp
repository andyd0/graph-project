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
void shortest_list(vector<vector<int> > permute_list, int **cities);
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

	char *numbers = new char[n];
	for (int i = 0; i < (n - 1); i++) {
		numbers[i] = (i + 1) + '0';
	}
	numbers[n - 1] = 0;

	int cost = INT_MAX;
	vector<int> shortest_path(n, 0);
	int i;
	int j;

	omp_set_num_threads(threads);
	// # pragma omp parallel num_threads(16) default(none) shared(cost, fact, cities, shortest_path, n) private(permutation, i, j)
	// {
	# pragma omp parallel for
	for(i = 0; i < fact; i++) {
		printf("index: %d and thread number %d\n", i, omp_get_thread_num());
		vector<int> permutation = ithPermutation(n , i);
		for (j = 0; j < n; j++) {
			int temp = check_graph(permutation, cities, cost);
			if (cost > temp) {
				cost = temp;
				shortest_path = permutation;
			}
		}	
	}
	// }

	cout << "Shortest path is:" << endl;
	for (int x : shortest_path)
		printf("%d ", x);
	printf("\ntotal weight = %d\n", cost);

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

void permutations(char *input, int l, int r, int n, int &j, vector<vector<int> > &permute_list) {

	if (l == r) {
		vector<int> temp(n);
		temp.at(0) = 0;
		for (int i = 0; i < n - 1; i++)
			temp.at(i + 1) = input[i] - '0';
		permute_list.at(j) = temp;
		j++;
	}
	else {
		for (int i = l; i <= r; i++) {
			swap((input + l), (input + i));
			permutations(input, l + 1, r, n, j, permute_list);
			swap((input + l), (input + i));
		}
	}
};


vector<int> ithPermutation(const int n, int i)
{
   int j, k = 0;
   vector<int> factorials(n);
   vector<int> permutation(n);

   factorials.at(k) = 1;
   while (++k < n)
      factorials.at(k) = factorials.at(k - 1) * k;

   for (k = 1; k < n; ++k)
   {
      permutation.at(k) = i / factorials.at(n - 1 - k);
      i = i % factorials.at(n - 1 - k);
   }

   for (k = n - 1; k > 0; --k)
      for (j = k - 1; j >= 0; --j)
         if (permutation.at(j) <= permutation.at(k))
            permutation.at(k)++;
	
   	return permutation;
}


void shortest_list(vector<vector<int> > permute_list, int **cities) {

	vector<int> shortest_path;
	int cost = INT_MAX;
	int n = permute_list.size();
	int i;

	omp_set_num_threads(32);
	# pragma omp parallel for
	for (i = 0; i < n; i++) {
		//printf("index: %d and thread number %d\n", i, omp_get_thread_num());
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
