#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <list>
#include <sstream>
#include <string>
#include "graph.h"

Graph::Graph(int vertex_count) {
	this->vertex_count = vertex_count;
	adj = (int **)malloc(vertex_count * sizeof(int *));
	out_edges = new int[vertex_count];
	in_edges = new int[vertex_count];
}

void Graph::generate(std::ifstream &inputFile, std::string algorithm) {

	std::clock_t start;
	start = std::clock();

	// Assumed max number of edges
	int DEG = 103000;

	int *check_edges = new int[vertex_count];

	for (int i = 0; i < vertex_count; i++) {
		in_edges[i] = 0;
		out_edges[i] = 0;
		check_edges[i] = 0;
	}

	for (int i = 0; i < vertex_count; i++) {
		adj[i] = (int *)malloc(DEG * sizeof(int));
	}

	int u;
	int v;
	
	while (inputFile >> u && inputFile >> v) {

		// If the algorithm is Pagerank, the vertices should be flipped
		// to account for incoming edges.  Still out edges should be
		// tracked regardless due to computation
		if (algorithm == PR_ALGO)
		{
			if(check_edges[v] <= DEG) {
				adj[v][check_edges[v]] = u;
				check_edges[v]++;
			}
		}
		else {
			if(check_edges[u] <= DEG) {
				adj[u][check_edges[u]] = v;
				check_edges[u]++;
			}
		}
		out_edges[u]++;
		in_edges[v]++;
	}
	time_to_generate = (std::clock() - start) / (double)CLOCKS_PER_SEC;
}

int Graph::vertexCount() {
	return vertex_count;
}

int* Graph::getAdjList(int u) {
	return adj[u];
}

int Graph::getFromAdjList(int u, int i) {
	return adj[u][i];
}

int Graph::getOutEdges(int u) {
	return out_edges[u];
}

int* Graph::getOutEdgesArray() {
	return out_edges;
}

int* Graph::getInEdgesArray() {
	return in_edges;
}

double Graph::getTimeToGenerate() {
	return time_to_generate;
}