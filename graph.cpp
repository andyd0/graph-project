#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <list>
#include <sstream>
#include <string>
#include "graph.h"

Graph::Graph(int vertex_count, std::string graph_type) {
	this->vertex_count = vertex_count;
	this->graph_type = graph_type;
	adj = new std::list<int>[vertex_count];
	out_edges = new int[vertex_count];
}

void Graph::generate(std::ifstream &inputFile, std::string algorithm) {

	int u;
	int v;

	std::clock_t start;
	start = std::clock();

	for(int i = 0; i < vertex_count; i++)
		out_edges[i] = 0;

	while (inputFile >> u && inputFile >> v) {

		// If the algorithm is Pagerank, the vertices should be flipped
		// to account for incoming edges.  Still out edges should be
		// tracked regardless due to computation
		(algorithm == PR_ALGO)	? addEdge(v, u) : addEdge(u, v);
		out_edges[u]++;
		
		// Since the edges have no direction, both sides of the edge
		// should be accounted for
		if(graph_type == UNDIRECTED) {
			addEdge(v, u);
			out_edges[v]++;
		}
	}
	time_to_generate = ( std::clock() - start ) / (double) CLOCKS_PER_SEC / 1000;
}

int Graph::vertexCount() {
	return vertex_count;
}

void Graph::addEdge(int v, int w) {
	adj[v].push_back(w);
}

std::list<int> Graph::getAdj(int u) {
	return adj[u];
}

int* Graph::getOutEdges() {
	return out_edges;
}

double Graph::getTimeToGenerate() {
	return time_to_generate;
}