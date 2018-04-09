#ifndef GRAPH
#define GRAPH

#include <fstream>
#include <list>

#define PR_ALGO "pr"
#define BFS_ALGO "bfs"

#define DIRECTED "d"
#define UNDIRECTED "u"

class Graph {
	int vertex_count;
	std::string graph_type;
	std::list<int> *adj;
	int *out_edges;

public:
	Graph(int vertex_count, std::string graph_type);
	int vertexCount();
	void addEdge(int v, int w);
	void generate(std::ifstream &inputFile, std::string algorithm);
	std::list<int> getAdj(int u);
};

#endif