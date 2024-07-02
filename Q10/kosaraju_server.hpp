#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <list>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>

class Graph {
public:
    Graph(int n) : adj(n), n(n) {}  // Constructor to initialize the graph with n vertices

    // Method to add an edge from u to v
    void addEdge(int u, int v) {
        adj[u - 1].push_back(v - 1);
    }

    // Method to remove an edge from u to v
    void removeEdge(int u, int v) {
        adj[u - 1].remove(v - 1);
    }

    // Method to compute and return all SCCs using Kosaraju's algorithm
    std::string kosaraju(bool& condition_met) {
        std::vector<std::vector<int>> sccs;  // Vector to store all SCCs
        std::vector<bool> visited(n, false);  // Visited array to keep track of visited vertices
        std::stack<int> Stack;  // Stack to store the order of vertices by finishing times

        // Perform DFS for each vertex to fill the stack
        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                fillOrder(i, visited, Stack);
            }
        }

        std::list<int>* transposedAdj = getTranspose();  // Get the transposed graph

        std::fill(visited.begin(), visited.end(), false);  // Mark all vertices as not visited for the second DFS

        // Process all vertices in the order defined by the stack
        while (!Stack.empty()) {
            int v = Stack.top();
            Stack.pop();

            if (!visited[v]) {
                std::vector<int> component;  // Vector to store the current SCC
                DFSUtil(v, visited, transposedAdj, component);  // Perform DFS on the transposed graph
                sccs.push_back(component);
            }
        }

        delete[] transposedAdj;  // Free the memory used by the transposed graph

        // Prepare the SCCs result string
        std::stringstream ss;
        ss << "Total number of SCCs: " << sccs.size() << std::endl;
        int max_scc_size = 0;
        for (int i = 0; i < sccs.size(); ++i) {
            ss << "SCC " << (i + 1) << " is: ";
            for (int vertex : sccs[i]) {
                ss << (vertex + 1) << " ";
            }
            max_scc_size = std::max(max_scc_size, (int)sccs[i].size());
            ss << std::endl;
        }

        // Check if at least 50% of the graph vertices are in one SCC
        condition_met = (max_scc_size >= (n + 1) / 2);  // Ensure rounding up for odd n

        return ss.str();
    }

private:
    std::vector<std::list<int>> adj;  // Adjacency list representation of the graph
    int n;  // Number of vertices in the graph

    // Helper method to perform DFS and fill the stack
    void fillOrder(int v, std::vector<bool>& visited, std::stack<int>& Stack) {
        visited[v] = true;  // Mark the current node as visited
        for (int neighbor : adj[v]) {  // Iterate over all the adjacent vertices
            if (!visited[neighbor]) {  // If the adjacent vertex has not been visited
                fillOrder(neighbor, visited, Stack);  // Recursive call for the adjacent vertex
            }
        }
        Stack.push(v);  // Push the vertex to the stack after all its adjacent vertices are processed
    }

    // Helper method to perform DFS on the transposed graph
    void DFSUtil(int v, std::vector<bool>& visited, std::list<int>* transposedAdj, std::vector<int>& component) {
        visited[v] = true;  // Mark the current node as visited
        component.push_back(v);  // Add this vertex to the current component
        for (int neighbor : transposedAdj[v]) {  // Iterate over all the adjacent vertices in the transposed graph
            if (!visited[neighbor]) {  // If the adjacent vertex has not been visited
                DFSUtil(neighbor, visited, transposedAdj, component);  // Recursive call for the adjacent vertex
            }
        }
    }

    // Helper method to get the transposed graph
    std::list<int>* getTranspose() {
        std::list<int>* transposedAdj = new std::list<int>[n];  // Create an empty transposed graph
        for (int v = 0; v < n; ++v) {  // Iterate over each vertex
            for (int neighbor : adj[v]) {  // Iterate over all the adjacent vertices
                transposedAdj[neighbor].push_back(v);  // Add an edge from neighbor to v in the transposed graph
            }
        }
        return transposedAdj;  // Return the transposed graph
    }
};

#endif // GRAPH_HPP
