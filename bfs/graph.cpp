#include <cstdlib>
#include <fstream>
#include <list>
#include <sstream>
#include <string>
#include "graph.h"

Graph::Graph(int V) {
	this->V = V;
	adj = new std::list<int>[V];
}

void Graph::generate(std::ifstream &inputFile) {

	int v;
	int w;

	while(!inputFile.eof()){
		inputFile >> v;
		inputFile >> w;
		addEdge(v, w);
	}
}

int Graph::vertexCount() {
	return V;
}

void Graph::addEdge(int v, int w) {
	adj[v].push_back(w);
}

std::list<int> Graph::getAdj(int u) {
	return adj[u];
}
