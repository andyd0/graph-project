#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <omp.h>

using namespace std;

void swap(char *a, char *b);
void permutations(char *input, int l, int r, int &j, vector<vector<int> > &permute_list);
int factorial(int n);

int main(int argc, char *argv[]) {

    if(argc < 4) { 
        printf("Please use the right number of arguments\n");
        exit(0);
    }

    int n = atoi(argv[1]);
    int threads = atoi(argv[2]);

    printf("n: %d and threads: %d\n", n, threads);

    int **cities = new int*[n];
    for(int i = 0; i < n; i++)
        cities[i] = new int[n];
    
    string line;

    ifstream inputFile(argv[3]);

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            inputFile >> cities[i][j];
        }
    }

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            cout << cities[i][j] << " ";
        }
        cout << endl;
    }

    int fact = factorial(n);
    vector<vector<int> > permute_list(fact, vector<int>(n - 1, 0));    

    char *numbers = new char[n];
    for(int i = 0; i < (n - 1); i++) {
        numbers[i] = (i + 1) + '0';
    }
    numbers += '\0';

    int j = 0;
    permutations(numbers, 0, n - 2, j, permute_list);

    printf("fact: %d, size: %d, index: %d\n", fact, static_cast<int>(permute_list.size()), j);
}

void swap(char *a, char *b) {
    char temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

int factorial(int n) {
    int value = 1;
    for(int i = n - 1; i > 1; i--)
        value *= i;
    return value;
}

void permutations(char *input, int l, int r, int &j, vector<vector<int> > &permute_list) {

    int n = sizeof(input)/sizeof(*input);

    if(l == r) {
        vector<int> temp(n);
        for(int i = 0; i < n; i++)
            temp.at(i) = input[i];
        permute_list.at(j) = temp;
        j++;
    } else {
        for(int i = l; i <= r; i++) {
            swap((input + l), (input + i));
            permutations(input, l+1, r, j, permute_list);
            swap((input + l), (input + i));
        }
    }
};


