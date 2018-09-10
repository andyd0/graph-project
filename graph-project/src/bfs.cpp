#include <ctime>
#include <iomanip>
#include <iostream>
#include <list>
#include <omp.h>
#include <queue>
#include <vector>
#include <string>
#include "bitmap.h"
#include "graph.h"
#include <tbb/concurrent_queue.h>

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
		for (int i = 0; i < G.getOutEdges(u); i++) {
			int v = G.getFromAdjList(u, i);
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
		for (int i = 0; i < nodes_in_queue; i++) {
			int u;
			# pragma omp critical
			{
				u = currentFrontier.front();
				currentFrontier.pop();
			}

			for (int i = 0; i < G.getOutEdges(u); i++) {
				int v = G.getFromAdjList(u, i);
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


int* BFS_Parallel_Bitmap(Graph G, int source, int threads) {

	int num_vertices = G.vertexCount();
	int* parents = new int[num_vertices];

	for (int i = 0; i < num_vertices; i++)
		parents[i] = -1;

	parents[source] = 0;
	std::queue<int> currentFrontier;
	currentFrontier.push(source);
	Bitmap bitCheck(num_vertices);
	bitCheck.reset();
	bitCheck.set_bit(source);

	omp_set_num_threads(threads);

	while (!currentFrontier.empty()) {

		// Since OpenMP will not work with While loops a for loop is
		// introduced here that cycles through the number of nodes in the
		// queue currently
		int nodes_in_queue = currentFrontier.size();

		# pragma omp parallel for
		for (int i = 0; i < nodes_in_queue; i++) {
			int u;
			# pragma omp critical
			{
				u = currentFrontier.front();
				currentFrontier.pop();
			}

			for (int i = 0; i < G.getOutEdges(u); i++) {
				int v = G.getFromAdjList(u, i);
				if (!bitCheck.get_bit(v)) {
					parents[v] = u;
					bitCheck.set_bit_atomic(v);
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


int* BFS_Parallel_Concurrent(Graph G, int source, int threads) {

	int num_vertices = G.vertexCount();
	int* parents = new int[num_vertices];

	for (int i = 0; i < num_vertices; i++)
		parents[i] = -1;

	parents[source] = 0;
	tbb::concurrent_bounded_queue<int> currentFrontier;
	currentFrontier.push(source);
	Bitmap bitCheck(num_vertices);
	bitCheck.reset();
	bitCheck.set_bit(source);

	omp_set_num_threads(threads);

	while (!currentFrontier.empty()) {

		// Since OpenMP will not work with While loops a for loop is
		// introduced here that cycles through the number of nodes in the
		// queue currently
		int nodes_in_queue = currentFrontier.size();

		# pragma omp parallel for
		for (int i = 0; i < nodes_in_queue; i++) {
			int u;
			currentFrontier.pop(u);

			for (int i = 0; i < G.getOutEdges(u); i++) {
				int v = G.getFromAdjList(u, i);
				if (!bitCheck.get_bit(v)) {
					parents[v] = u;
					bitCheck.set_bit(v);
					currentFrontier.push(v);
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
		for (int i = 0; i < G.getOutEdges(u); i++) {
			int v = G.getFromAdjList(u, i);
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
			for (int i = 0; i < G.getOutEdges(u); i++) {
			int v = G.getFromAdjList(u, i);
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
		out << "v " << i << " parent " << parents[i] << std::endl;
	}
};

int main(int argc, char *argv[]) {

	int vertex_count = atoi(argv[1]);
	int source = atoi(argv[2]);
	int parallel = atoi(argv[3]);
	int threads = atoi(argv[4]);
	int verify = atoi(argv[5]);
	int saveTofile = atoi(argv[6]);
	std::ifstream inputFile(argv[7]);

	Graph G(vertex_count);
	G.generate(inputFile, BFS_ALGO);

	printf("Time to process graph: %fs\n", G.getTimeToGenerate());

	double start_process = omp_get_wtime();
	std::string process_type = "";

	int* parents;

	if (parallel == 1) {
		parents = BFS_Parallel(G, source, threads);
		process_type = "parallel";
	}
	else if(parallel == 2) {
		parents = BFS_Parallel_Bitmap(G, source, threads);
		process_type = "parallel with bitmap";
	}
	else if(parallel == 3) {
		parents = BFS_Parallel_Concurrent(G, source, threads);
		process_type = "parallel with bitmap and concurrent queues";
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

	if (saveTofile)
		saveResultsToFile(parents, vertex_count);

	delete(parents);
}