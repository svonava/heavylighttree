== What is this
Implementation of tree data structure that supports CHANGE and QUERY operations as defined below.

Given a tree (an acyclic undirected connected graph) with N nodes, and edges numbered 1, 2, 3...N-1.
- CHANGE i ti: change the cost of the i-th edge to ti
- QUERY a b: ask for the maximum edge cost on the path from node a to node b

== How to run this
1. Install gcc 9.4.0 (ubuntu package called g++, version: 4:9.3.0-1ubuntu2)
2. Compile with g++ main.cpp
3. Run with ./a.out < in.txt > out.txt
