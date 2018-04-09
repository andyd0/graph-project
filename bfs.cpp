#include <iostream>
#include <limits>
#include <list>
#include <omp.h>
#include <queue>
#include <vector>
#include <string>
#include "graph.h"

// Sequential version of BFS which is based on 
// CLRS' algorithm
int* BFS(Graph G, int source) {

	int num_vertices = G.vertexCount();
	int* parents = new int[num_vertices];
	int maxInt = std::numeric_limits<int>::max();

	for (int i = 0; i < num_vertices; i++)
		parents[i] = maxInt;

	parents[source] = 0;
	std::queue<int> currentFrontier;
	currentFrontier.push(source);

	while (!currentFrontier.empty()) {
		int u = currentFrontier.front();
		currentFrontier.pop();
		for (int v : G.getAdj(u)) {
			if (parents[v] == maxInt) {
				parents[v] = u;
				currentFrontier.push(v);
			}
		}
	}
	return parents;
}

int* BFSP(Graph G, int source, int threads) {

	int num_vertices = G.vertexCount();
	int* parents = new int[num_vertices];
	int maxInt = std::numeric_limits<int>::max();

	for (int i = 0; i < num_vertices; i++)
		parents[i] = maxInt;

	parents[source] = 0;
	std::queue<int> currentFrontier;
	currentFrontier.push(source);

	omp_set_num_threads(threads);

	while (!currentFrontier.empty()) {

		// Since OpenMP will not work with While loops a for loop is
		// introduced here that cycles through the number of nodes in the
		// queue currently
		int nodes_in_queue = currentFrontier.size();
		# pragma omp parallel for
		for(int i = 0; i < nodes_in_queue; i++) {
			int u;
			# pragma omp critical
			{
				u = currentFrontier.front();
				currentFrontier.pop();
			}

			for (int v : G.getAdj(u)) {
				if (parents[v] == maxInt) {
					parents[v] = u;
					# pragma omp critical
					{
						currentFrontier.push(v);
					}
				}
			}
		}
	}
	return parents;
}

bool verifyBFSTree(Graph G, int source, int *parents) {
	int num_vertices = G.vertexCount();
	std::vector<int> depth(num_vertices, -1);
	depth.at(source) = 0;

	std::queue<int> currentFrontier;
	currentFrontier.push(source);

	while (!currentFrontier.empty()) {
		int u = currentFrontier.front();
		currentFrontier.pop();
		for (int v : G.getAdj(u)) {
			if (depth.at(v) == -1) {
				depth.at(v) = depth.at(u) + 1;
				currentFrontier.push(v);
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

	std::string graph_type = argv[1];
	std::string algorithm = argv[2];
	int vertex_count = atoi(argv[3]);
	int source = atoi(argv[4]);
	int threads = atoi(argv[5]);
	int verify = atoi(argv[6]);
	std::ifstream inputFile(argv[7]);

	Graph G(vertex_count, graph_type);
	G.generate(inputFile, algorithm);

	// int* parents = BFS(G, source);

	int* parents = BFSP(G, source, threads);

	// for (int i = 0; i < V; i++)
	// 	std::cout << parents[i] << " ";
	// std::cout << std::endl;

	bool validTree = false;
	if (verify == 1) { 
		validTree = verifyBFSTree(G, source, parents);
		std::string test = (validTree) ? "True" : "False";
		std::cout << "Valid tree?: " << test << std::endl;
	}

	delete(parents);
}