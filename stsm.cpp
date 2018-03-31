#include <climits>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <omp.h>

using namespace std;

int check_graph(string path, int **cities, int cost);
void swap(char *a, char *b);
void permutations(char *input, int l, int r, int n, int &j, vector<string> &permute_list);
int factorial(int n);

std::vector<vector<int> > permute_list;

// Naive Implementation that generates all the permutations first via recursion and
// then checks each to find the minimum path.
int main(int argc, char *argv[]) {

	if (argc < 3) {
		printf("Please use the right number of arguments\n");
		exit(0);
	}

	int n = atoi(argv[1]);

	int **cities = new int*[n];
	for (int i = 0; i < n; i++)
		cities[i] = new int[n];

	ifstream inputFile(argv[2]);

	// Creating matrix from input
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			inputFile >> cities[i][j];
		}
	}

	// creating char array of numbers for permutations
	char *numbers = new char[n];
	for (int i = 0; i < (n - 1); i++) {
		numbers[i] = (i + 1) + '0';
	}
	numbers[n - 1] = 0;

	int fact = factorial(n);
	vector<string> permute_list(fact);
	int j = 0;

	// Generating permutations
	permutations(numbers, 0, n - 2, n, j, permute_list);

	int cost = INT_MAX;
	string shortest_path;
	int length = permute_list.size();

	// Checking what is the path with the least weight
	// omp_set_num_threads(10);
	// # pragma omp parallel for
	for (int i = 0; i < length; i++) {
		int temp = check_graph(permute_list.at(i), cities, cost);
		printf("%d\n", temp);
		if (cost > temp) {
			cost = temp;
			shortest_path = permute_list.at(i);
		}
	}

	cout << "Shortest path is:" << endl;
	for(char c : shortest_path)
		cout << c << " ";
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

// Generates permutations recursively in lexicographical order
void permutations(char *input, int l, int r, int n, int &j, vector<string> &permute_list) {

	if (l == r) {
		string temp(input);
		permute_list.at(j) = "0" + temp;
		j++;
	}
	else {
		for (int i = l; i <= r; i++) {
			swap((input + l), (input + i));
			permutations(input, l + 1, r, n, j, permute_list);
			swap((input + l), (input + i));
		}
	}
}

// Fucntions return the weight of the path but will return MAX
// if the path weight is actually larger than the current cost
int check_graph(string path, int **cities, int cost) {

	int path_sum = 0;
	int i = 0;
	int j = 1;
	int path_size = path.size();

	while (j < path_size) {
		path_sum += cities[path[i++] - '0'][path[j++] - '0'];
		if (path_sum > cost) {
			path_sum = INT_MAX;
			break;
		}
	}

	return path_sum;
}
