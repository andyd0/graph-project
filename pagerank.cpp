#include <iomanip>
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

double* PR_Parallel(Graph G, int iterations, int threads) {

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

		omp_set_num_threads(threads);
		# pragma omp parallel for
		for (int i = 0; i < vertex_count; i++) {
			std::list<int> incoming = G.getAdj(i);
			std::list<int>::iterator it;
			for (it = incoming.begin(); it != incoming.end(); ++it) {
				pr_temp[i] += pr_values[*it] / out_edges[*it];
			}
			pr_temp[i] = pr_temp[i] * damping_factor + adj;
		}

		# pragma omp barrier
		for (int i = 0; i < vertex_count; i++) {
			pr_values[i] = pr_temp[i];
			pr_temp[i] = 0;
		}
		current++;
	}

	delete(pr_temp);

	return pr_values;
}

void saveResultsToFile(double *pr_values, int vertex_count) {
	std::ofstream out("pagerank_values.txt");
	
	for (int i = 0; i < vertex_count; i++) {
		out << "vertex " << i << " has pagerank of " << std::setprecision(10) << std::fixed
		    << pr_values[i] << std::endl;
	}
};

int main(int argc, char *argv[]) {

	std::string graph_type = argv[1];
	int vertex_count = atoi(argv[2]);
	int iterations = atoi(argv[3]);
	int parallel = atoi(argv[4]);
	int threads = atoi(argv[5]);
	int saveTofile = atoi(argv[6]);
	std::ifstream inputFile(argv[7]);

	if(!inputFile.good()) {
		std::cout << "\nPlease be sure to use an input file" << std::endl;
		return 0;
	}

	Graph G(vertex_count, graph_type);
	G.generate(inputFile, PR_ALGO);

	double* pr_values;

	if(parallel) {
		pr_values = PR_Parallel(G, iterations, threads);
		std::cout << "Parallel computation" << std::endl;
	}
	else {
		pr_values = PR(G, iterations);
		std::cout << "Sequential computation" << std::endl;
	}

	if(saveTofile)
		saveResultsToFile(pr_values, vertex_count);

	delete(pr_values);
}