#include <iostream>
#include <limits>
#include <list>
#include <queue>
#include "graph.h"

int* BFS(Graph G, int V, int r) {

	int* parents = new int[V];
	int maxInt = std::numeric_limits<int>::max();

	for (int i = 0; i < V; i++)
		parents[i] = maxInt;

	parents[r] = 0;
	std::queue<int> currentQueue;
	currentQueue.push(r);
	std::queue<int> nextQueue;

	while (!currentQueue.empty()) {
		nextQueue = std::queue<int>();
		while (!currentQueue.empty()) {
			int u = currentQueue.front();
			currentQueue.pop();
			for (int v : G.getAdj(u)) {
				if (parents[v] == maxInt) {
					parents[v] = u;
					nextQueue.push(v);
				}
			}
		}
		currentQueue = nextQueue;
	}
	return parents;
}