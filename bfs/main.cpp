#include <fstream>
#include <iostream>
#include "bfs.h"
#include "graph.h"

int main(int argc, char *argv[]) {

	int V = atoi(argv[1]);
    std::ifstream inputFile(argv[2]);

	Graph G(V);
    G.generate(inputFile);

	int* parents = BFS(G, V, 0);
	for (int i = 0; i < V; i++)
		std::cout << parents[i] << " ";
	std::cout << std::endl;
}