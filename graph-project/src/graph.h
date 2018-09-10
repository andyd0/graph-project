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
	int** adj;
	int* out_edges;
	int* in_edges;
	double time_to_generate;

public:
	Graph(int vertex_count);
	int vertexCount();

	void generate(std::ifstream &inputFile, std::string algorithm);
	void addEdge(int v, int w);
	int* getAdjList(int u);
	int getFromAdjList(int u, int i);
	int getOutEdges(int u);
	int* getOutEdgesArray();
	int* getInEdgesArray();
	double getTimeToGenerate();
};

#endif