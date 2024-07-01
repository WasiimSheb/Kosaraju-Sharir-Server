// kosarajuVectorVec.cpp
// This file implements Kosaraju's algorithm for finding strongly connected components (SCCs) in a directed graph
// using an adjacency list representation with a vector of vectors. The program reads a graph from an input file, performs
// Kosaraju's algorithm, and prints out the SCCs.

#include "kosaraju_scc.hpp"

using namespace std;

// Function to perform DFS and fill the stack
void fillOrderVectorVec(int v, vector<bool>& visited, stack<int>& Stack, const vector<vector<int>>& adj) {
    visited[v] = true;  // Mark the current node as visited
    for (int neighbor : adj[v]) {  // Iterate over all the adjacent vertices
        if (!visited[neighbor]) {  // If the adjacent vertex has not been visited
            fillOrderVectorVec(neighbor, visited, Stack, adj);  // Recursive call for the adjacent vertex
        }
    }
    Stack.push(v);  // Push the vertex to the stack after all its adjacent vertices are processed
}

// Function to perform DFS on the transposed graph
void DFSUtilVectorVec(int v, vector<bool>& visited, const vector<vector<int>>& transposedAdj, vector<int>& component) {
    visited[v] = true;  // Mark the current node as visited
    component.push_back(v);  // Add this vertex to the current component
    for (int neighbor : transposedAdj[v]) {  // Iterate over all the adjacent vertices in the transposed graph
        if (!visited[neighbor]) {  // If the adjacent vertex has not been visited
            DFSUtilVectorVec(neighbor, visited, transposedAdj, component);  // Recursive call for the adjacent vertex
        }
    }
}

// Function to get the transposed graph
vector<vector<int>> getTransposeVectorVec(const vector<vector<int>>& adj) {
    size_t n = adj.size();
    vector<vector<int>> transposedAdj(n);  // Create an empty transposed graph
    for (size_t v = 0; v < n; ++v) {  // Iterate over each vertex
        for (int neighbor : adj[v]) {  // Iterate over all the adjacent vertices
            transposedAdj[neighbor].push_back(v);  // Add an edge from neighbor to v in the transposed graph
        }
    }
    return transposedAdj;  // Return the transposed graph
}

// Function to find and print all strongly connected components
void findSCCsVectorVec(int n, int m, const vector<pair<int, int>>& edges) {
    vector<vector<int>> adj(n);  // Adjacency list representation of the graph
    for (const auto& edge : edges) {  // Iterate over all edges
        adj[edge.first - 1].push_back(edge.second - 1);  // Add edge to the adjacency list
    }

    stack<int> Stack;  // Stack to store the order of vertices by finishing times
    vector<bool> visited(n, false);  // Visited array to keep track of visited vertices

    for (int i = 0; i < n; ++i) {  // Perform DFS for each vertex
        if (!visited[i]) {  // If the vertex has not been visited
            fillOrderVectorVec(i, visited, Stack, adj);  // Perform DFS and fill the stack
        }
    }

    vector<vector<int>> transposedAdj = getTransposeVectorVec(adj);  // Get the transposed graph

    fill(visited.begin(), visited.end(), false);  // Mark all vertices as not visited for the second DFS

    int sccCount = 0;  // Counter for SCCs
    vector<vector<int>> sccs;  // To store all SCCs

    while (!Stack.empty()) {  // Process all vertices in the order defined by the stack
        int v = Stack.top();  // Get the top vertex
        Stack.pop();  // Remove the top vertex

        if (!visited[v]) {  // If the vertex has not been visited
            vector<int> component;  // To store the current strongly connected component
            DFSUtilVectorVec(v, visited, transposedAdj, component);  // Perform DFS on the transposed graph
            sccs.push_back(component);
            sccCount++;
        }
    }

    cout << "Total number of SCCs: " << sccCount << endl;
    for (int i = 0; i < sccCount; ++i) {
        cout << "SCC " << (i + 1) << " is: ";
        for (int vertex : sccs[i]) {
            cout << (vertex + 1) << " ";
        }
        cout << endl;
    }
}

int main() {
    ifstream infile("graph.txt");  // Open the input file
    if (!infile) {
        cerr << "Error opening input file" << endl;
        return 1;
    }

    int n, m;
    infile >> n >> m;  // Read the number of vertices and edges
    if (n <= 0 || m <= 0) {
        cerr << "Invalid number of vertices or edges" << endl;
        return 1;
    }

    vector<pair<int, int>> edges(m);  // Vector to store all the edges
    for (int i = 0; i < m; i++) {
        infile >> edges[i].first >> edges[i].second;  // Read each edge
        if (edges[i].first <= 0 || edges[i].first > n || edges[i].second <= 0 || edges[i].second > n) {
            cerr << "Invalid edge: " << edges[i].first << " " << edges[i].second << endl;
            return 1;
        }
    }

    findSCCsVectorVec(n, m, edges);  // Find and print all strongly connected components

    return 0;
}
