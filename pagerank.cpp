#include <iostream>
#include <limits>
#include <list>
#include <omp.h>
#include <queue>
#include <vector>
#include <string>
#include "graph.h"

double* PR(Graph G, int iterations) {

	int vertex_count = G.vertexCount();

	double* pr_values = new double[vertex_count];
	double* pr_temp = new double[vertex_count];


	double pr_initial = 1 / (double)vertex_count;

	for (int i = 0; i < vertex_count; i++) {
		pr_values[i] = pr_initial;
		pr_temp[i] = 0;
	}
		
	int current = 0;
	double damping_factor = .80;
	double adj = (1 - damping_factor) / (double)vertex_count;
	int *out_edges = G.getOutEdges();

	while (current < iterations) {

		for (int i = 0; i < vertex_count; i++) {
			std::list<int> incoming = G.getAdj(i);
			std::list<int>::iterator it;
			for (it = incoming.begin(); it != incoming.end(); ++it) {
				pr_temp[i] += pr_values[*it] / out_edges[*it];
			}
			pr_temp[i] = pr_temp[i] * damping_factor + adj;
		}

		for (int i = 0; i < vertex_count; i++) {
			pr_values[i] = pr_temp[i];
			pr_temp[i] = 0;
		}
		current++;
	}

	delete(pr_temp);

	return pr_values;
}

int main(int argc, char *argv[]) {

	std::string graph_type = argv[1];
	std::string algorithm = argv[2];
	int vertex_count = atoi(argv[3]);
	int iterations = atoi(argv[4]);
	int threads = atoi(argv[5]);
	int verify = atoi(argv[6]);
	std::ifstream inputFile(argv[7]);

	Graph G(vertex_count, graph_type);
	G.generate(inputFile, algorithm);

	double* pr_values = PR(G, iterations);

	delete(pr_values);
}