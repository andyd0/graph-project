#include <iostream>
#include <limits>
#include <list>
#include <omp.h>
#include <queue>
#include <vector>
#include <string>
#include "graph.h"

// Based on the CLRS algorithm
int* BFS(Graph G, int source) {

	int num_vertices = G.vertexCount();
	int* parents = new int[num_vertices];
	int maxInt = std::numeric_limits<int>::max();

	for (int i = 0; i < num_vertices; i++)
		parents[i] = maxInt;

	parents[source] = 0;
	std::queue<int> currentLevel;
	currentLevel.push(source);

	while (!currentLevel.empty()) {
		int u = currentLevel.front();
		currentLevel.pop();
		for (int v : G.getAdj(u)) {
			if (parents[v] == maxInt) {
				parents[v] = u;
				currentLevel.push(v);
			}
		}
	}
	return parents;
}

int* BFSP(Graph G, int r) {

	int num_vertices = G.vertexCount();
	int* parents = new int[num_vertices];
	int maxInt = std::numeric_limits<int>::max();

	for (int i = 0; i < num_vertices; i++)
		parents[i] = maxInt;

	parents[r] = 0;
	std::queue<int> currentLevel;
	currentLevel.push(r);

	while (!currentLevel.empty()) {
		int u = currentLevel.front();
		currentLevel.pop();
		for (int v : G.getAdj(u)) {
			if (parents[v] == maxInt) {
				parents[v] = u;
				currentLevel.push(v);
			}
		}
	}
	return parents;
}

bool verifyBFSTree(Graph G, int source, int *parents) {
	int num_vertices = G.vertexCount();
	std::vector<int> depth(num_vertices, -1);
	depth.at(source) = 0;

	std::queue<int> currentLevel;
	currentLevel.push(source);

	while (!currentLevel.empty()) {
		int u = currentLevel.front();
		currentLevel.pop();
		for (int v : G.getAdj(u)) {
			if (depth.at(v) == -1) {
				depth.at(v) = depth.at(u) + 1;
				currentLevel.push(v);
			}
		}
	}

	for (int u = 0; u < num_vertices; u++) {
		if (u == source) {
			continue;
		}
		else {
			for (int v : G.getAdj(u)) {
				if (parents[v] == u) {
					if (depth[v] != depth[u] + 1)
						return false;
					else
						continue;
				}
			}
		}
	}
	return true;
}

int main(int argc, char *argv[]) {

	int V = atoi(argv[1]);
	int source = atoi(argv[2]);
	std::ifstream inputFile(argv[3]);
	
	int verify = 0;

	if(argc == 5)
		verify = atoi(argv[4]);

	Graph G(V);
	G.generate(inputFile);

	int* parents = BFSP(G, source);

	for (int i = 0; i < V; i++)
		std::cout << parents[i] << " ";
	std::cout << std::endl;

	bool validTree = false;
	if (verify == 1)
		validTree = verifyBFSTree(G, source, parents);
	std::string test = (validTree) ? "True" : "False";

	std::cout << "Valid tree?: " << test << std::endl;

	delete(parents);
}