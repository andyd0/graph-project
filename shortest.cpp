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

int main(int argc, char *argv[]) {

	if (argc < 4) {
		printf("Please use the right number of arguments\n");
		exit(0);
	}

	int n = atoi(argv[1]);
	int threads = atoi(argv[2]);

	printf("n: %d and threads: %d\n", n, threads);

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

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cout << cities[i][j] << " ";
		}
		cout << endl;
	}

	int fact = factorial(n);
	vector<vector<int> > permute_list(fact, vector<int>(n - 1, 0));

	char *numbers = new char[n];
	for (int i = 0; i < (n - 1); i++) {
		numbers[i] = (i + 1) + '0';
	}
	numbers[n - 1] = 0;

	int j = 0;
	permutations(numbers, 0, n - 2, n, j, permute_list);

	printf("fact: %d, size: %d, index: %d\n", fact, static_cast<int>(permute_list.size()), j);

	shortest_list(permute_list, cities);
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


void shortest_list(vector<vector<int> > permute_list, int **cities) {

	vector<int> shortest_path;
	int cost = INT_MAX;

	for (size_t i = 0; i < permute_list.size(); i++) {
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
			break;
		}
		x++;
		y++;
	}

	return path_sum;
}
