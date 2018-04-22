all: bfs pr

bfs: graph.cpp graph.h bfs.cpp
	g++ -Wall -fopenmp graph.cpp graph.h bfs.cpp -o bfs

pr: graph.cpp graph.h pagerank.cpp
	g++ -Wall -fopenmp graph.cpp graph.h pagerank.cpp -o pagerank

clean:
	rm -f bfs pagerank