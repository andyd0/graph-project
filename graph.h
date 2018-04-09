#ifndef GRAPH
#define GRAPH

#include <fstream>
#include <list>

class Graph {
	int V;
	std::list<int> *adj;

public:
	Graph(int V);
	int vertexCount();
	void addEdge(int v, int w);
	void generate(std::ifstream &inputFile);
	std::list<int> getAdj(int u);
};

#endif