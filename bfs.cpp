#include <ctime>
#include <iomanip>
#include <iostream>
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

	for (int i = 0; i < num_vertices; i++)
		parents[i] = -1;

	parents[source] = 0;
	std::queue<int> currentFrontier;
	currentFrontier.push(source);

	while (!currentFrontier.empty()) {
		int u = currentFrontier.front();
		currentFrontier.pop();
		for (int v : G.getAdj(u)) {
			if (parents[v] == -1) {
				parents[v] = u;
				currentFrontier.push(v);
			}
		}
	}
	return parents;
}

int* BFS_Parallel(Graph G, int source, int threads) {

	int num_vertices = G.vertexCount();
	int* parents = new int[num_vertices];

	for (int i = 0; i < num_vertices; i++)
		parents[i] = -1;

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
				if (parents[v] == -1) {
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

void saveResultsToFile(int* parents, int vertex_count) {
	std::ofstream out("bfs_tree_parents.txt");
	for (int i = 0; i < vertex_count; i++) {
		out << "vertex " << i << " has parent " << parents[i] << std::endl;
	}
};

int main(int argc, char *argv[]) {

	std::string graph_type = argv[1];
	int vertex_count = atoi(argv[2]);
	int source = atoi(argv[3]);
	int parallel = atoi(argv[4]);
	int threads = atoi(argv[5]);
	int verify = atoi(argv[6]);
	int saveTofile = atoi(argv[7]);
	std::ifstream inputFile(argv[8]);

	Graph G(vertex_count, graph_type);
	G.generate(inputFile, BFS_ALGO);

	printf("Time to process graph: %fs\n", G.getTimeToGenerate());

	double start_process = omp_get_wtime();
	std::string process_type = "";

	int* parents;

	if(parallel) {
		parents = BFS_Parallel(G, source, threads);
		process_type = "parallel";
	}
	else {
		parents = BFS(G, source);
		process_type = "sequential";
	}
	
	printf("Time to BFS for Parents (%s): %fs \n", process_type.c_str(), 
		   omp_get_wtime() - start_process);

	bool validTree = false;
	if (verify == 1) { 
		validTree = verifyBFSTree(G, source, parents);
		std::string test = (validTree) ? "True" : "False";
		std::cout << "Valid tree?: " << test << std::endl;
	}

	if(saveTofile)
		saveResultsToFile(parents, vertex_count);

	delete(parents);
}