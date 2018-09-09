#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <omp.h>
#include <queue>
#include <vector>
#include <string>
#include "graph.h"

double* PR(Graph G, int iterations, double damping_factor) {

	int vertex_count = G.vertexCount();

	double* pr_values = new double[vertex_count];
	double* pr_temp = new double[vertex_count];


	double pr_initial = 1 / (double)vertex_count;

	for (int i = 0; i < vertex_count; i++) {
		pr_values[i] = pr_initial;
		pr_temp[i] = 0;
	}

	int current = 0;
	double adj = (1 - damping_factor) / (double)vertex_count;
	int* out_edge_counts = G.getOutEdgesArray();
	int* in_edge_counts = G.getInEdgesArray();

	while (current < iterations) {

		for (int u = 0; u < vertex_count; u++) {
			int* incoming = G.getAdjList(u);
			for (int j = 0; j < in_edge_counts[u]; j++) {
				int v = incoming[j];
				pr_temp[u] += pr_values[v] / out_edge_counts[v];
			}
			pr_temp[u] = pr_temp[u] * damping_factor + adj;
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

double* PR_Parallel(Graph G, int iterations, double damping_factor, int threads) {

	int vertex_count = G.vertexCount();

	double* pr_values = new double[vertex_count];
	double* pr_temp = new double[vertex_count];


	double pr_initial = 1 / (double)vertex_count;

	# pragma omp parallel for
	for (int i = 0; i < vertex_count; i++) {
		pr_values[i] = pr_initial;
		pr_temp[i] = 0;
	}

	int current = 0;
	double adj = (1 - damping_factor) / (double)vertex_count;
	int* out_edge_counts = G.getOutEdgesArray();
	int* in_edge_counts = G.getInEdgesArray();

	while (current < iterations) {

		omp_set_num_threads(threads);
		# pragma omp parallel for
		for (int u = 0; u < vertex_count; u++) {
			int* incoming = G.getAdjList(u);
			for (int j = 0; j < in_edge_counts[u]; j++) {
				int v = incoming[j];
				pr_temp[u] += pr_values[v] / out_edge_counts[v];
			}
			pr_temp[u] = pr_temp[u] * damping_factor + adj;
		}

		# pragma omp parallel for
		for (int i = 0; i < vertex_count; i++) {
			pr_values[i] = pr_temp[i];
			pr_temp[i] = 0;
		}
		current++;
	}

	delete(pr_temp);

	return pr_values;
}

double* PR_Parallel_Revised(Graph G, int iterations, double damping_factor, int threads) {

	int vertex_count = G.vertexCount();

	double* pr_values = new double[vertex_count];
	double* pr_temp = new double[vertex_count];


	double pr_initial = 1 / (double)vertex_count;

	omp_set_num_threads(threads);
	
	# pragma omp parallel for
	for (int i = 0; i < vertex_count; i++) {
		pr_values[i] = pr_initial;
		pr_temp[i] = 0;
	}

	int current = 0;
	double adj = (1 - damping_factor) / (double)vertex_count;
	int* out_edge_counts = G.getOutEdgesArray();
	int* in_edge_counts = G.getInEdgesArray();
	double* contributions = new double[vertex_count];

	while (current < iterations) {

		omp_set_num_threads(threads);
		# pragma omp parallel for
		for (int u = 0; u < vertex_count; u++) {
			contributions[u] = pr_values[u] / out_edge_counts[u];
		}
		# pragma omp parallel for
		for (int u = 0; u < vertex_count; u++) {
			int* incoming = G.getAdjList(u);
			double sum = 0.0;
			for (int j = 0; j < in_edge_counts[u]; j++) {
				int v = incoming[j];
				sum += contributions[v];
			}
			pr_temp[u] = sum * damping_factor + adj;
		}

		# pragma omp parallel for
		for (int i = 0; i < vertex_count; i++) {
			pr_values[i] = pr_temp[i];
			pr_temp[i] = 0;
		}
		current++;
	}

	delete(pr_temp);

	return pr_values;
}

double* PR_Parallel_Rev_Scheduling(Graph G, int iterations, double damping_factor, int threads) {

	int vertex_count = G.vertexCount();

	double* pr_values = new double[vertex_count];
	double* pr_temp = new double[vertex_count];


	double pr_initial = 1 / (double)vertex_count;

	omp_set_num_threads(threads);
	
	# pragma omp parallel for
	for (int i = 0; i < vertex_count; i++) {
		pr_values[i] = pr_initial;
		pr_temp[i] = 0;
	}

	int current = 0;
	double adj = (1 - damping_factor) / (double)vertex_count;
	int* out_edge_counts = G.getOutEdgesArray();
	int* in_edge_counts = G.getInEdgesArray();
	double* contributions = new double[vertex_count];

	while (current < iterations) {

		omp_set_num_threads(threads);
		# pragma omp parallel for
		for (int u = 0; u < vertex_count; u++) {
			contributions[u] = pr_values[u]/out_edge_counts[u];
		}
		# pragma omp parallel for schedule(dynamic, 64)
		for (int u = 0; u < vertex_count; u++) {
			int* incoming = G.getAdjList(u);
			double sum = 0.0;
			for (int j = 0; j < in_edge_counts[u]; j++) {
				int v = incoming[j];
				sum += contributions[v];
			}
			pr_temp[u] = sum * damping_factor + adj;
		}

		# pragma omp parallel for
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
		out << "vertex " << i << " pagerank " << std::setprecision(10) << std::fixed
			<< pr_values[i] << std::endl;
	}
};

int main(int argc, char *argv[]) {

	int vertex_count = atoi(argv[1]);
	int iterations = atoi(argv[2]);
	double damping_factor = atof(argv[3]);
	int parallel = atoi(argv[4]);
	int threads = atoi(argv[5]);
	int saveTofile = atoi(argv[6]);
	std::ifstream inputFile(argv[7]);

	if (!inputFile.good()) {
		std::cout << "\nPlease be sure to use an input file" << std::endl;
		return 0;
	}

	Graph G(vertex_count);
	G.generate(inputFile, PR_ALGO);

	double* pr_values = new double[vertex_count];

	printf("Time to process graph: %fs\n", G.getTimeToGenerate());

	double start_process = omp_get_wtime();
	std::string process_type = "";

	if(parallel == 1){
		pr_values = PR_Parallel(G, iterations, damping_factor, threads);
		process_type = "parallel";
	}
	else if (parallel == 2) {
		pr_values = PR_Parallel_Revised(G, iterations, damping_factor, threads);
		process_type = "parallel revised algorithm";
	}
	else if(parallel == 3) {
		pr_values = PR_Parallel_Rev_Scheduling(G, iterations, damping_factor, threads);
		process_type = "parallel revised algorithm with scheduling";
	}
	else {
		pr_values = PR(G, iterations, damping_factor);
		process_type = "sequential";
	}

	printf("Time to compute Pagerank (%s): %fs \n", process_type.c_str(),
		omp_get_wtime() - start_process);

	if (saveTofile)
		saveResultsToFile(pr_values, vertex_count);

	delete(pr_values);
}