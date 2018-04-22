all: bfs pr

bfs: bitmap.h platform_atomics.h graph.cpp graph.h bfs.cpp
	g++ -std=c++11 -O3 -Wall -fopenmp bitmap.h graph.h graph.cpp bfs.cpp -o bfs

pr: graph.cpp graph.h pagerank.cpp
	g++ -Wall -fopenmp graph.cpp graph.h pagerank.cpp -o pagerank

clean:
	rm -f bfs pagerank