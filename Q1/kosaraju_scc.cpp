#include "kosaraju_scc.hpp"

// Function to perform DFS and fill the stack with vertices in the order of their finishing times
void fillOrder(int v, vector<bool>& visited, stack<int>& Stack, const unordered_map<int, list<int>>& adj) {
    visited[v] = true;  // Mark the current node as visited
    if (adj.find(v) != adj.end()) {  // Check if the vertex has any outgoing edges
        for (int neighbor : adj.at(v)) {  // Iterate over all the adjacent vertices
            if (!visited[neighbor]) {  // If the adjacent vertex has not been visited
                fillOrder(neighbor, visited, Stack, adj);  // Recursive call for the adjacent vertex
            }
        }
    }
    Stack.push(v);  // Push the vertex to the stack after all its adjacent vertices are processed
}

// Function to perform DFS on the transposed graph
void DFSUtil(int v, vector<bool>& visited, const unordered_map<int, list<int>>& transposedAdj, vector<int>& component) {
    visited[v] = true;  // Mark the current node as visited
    component.push_back(v);  // Add this vertex to the current component
    if (transposedAdj.find(v) != transposedAdj.end()) {  // Check if the vertex has any incoming edges in the transposed graph
        for (int neighbor : transposedAdj.at(v)) {  // Iterate over all the adjacent vertices in the transposed graph
            if (!visited[neighbor]) {  // If the adjacent vertex has not been visited
                DFSUtil(neighbor, visited, transposedAdj, component);  // Recursive call for the adjacent vertex
            }
        }
    }
}

// Function to get the transposed (reverse) graph
unordered_map<int, list<int>> getTranspose(int n, const unordered_map<int, list<int>>& adj) {
    unordered_map<int, list<int>> transposedAdj;  // Create an empty transposed graph
    for (int i = 1; i <= n; i++) {  // Iterate over each vertex
        if (adj.find(i) != adj.end()) {  // Check if the vertex has any outgoing edges
            for (int neighbor : adj.at(i)) {  // Iterate over all the adjacent vertices
                transposedAdj[neighbor].push_back(i);  // Add an edge from neighbor to i in the transposed graph
            }
        }
    }
    return transposedAdj;  // Return the transposed graph
}

// Function to find and print all strongly connected components
void findSCCs(int n, int m, const vector<pair<int, int>>& edges) {
    unordered_map<int, list<int>> adj;  // Adjacency list representation of the graph
    for (const auto& edge : edges) {  // Iterate over all edges
        adj[edge.first].push_back(edge.second);  // Add edge to the graph
    }

    stack<int> Stack;  // Stack to store the order of vertices by finishing times
    vector<bool> visited(n + 1, false);  // Visited array to keep track of visited vertices

    for (int i = 1; i <= n; i++) {  // Perform DFS for each vertex
        if (!visited[i]) {  // If the vertex has not been visited
            fillOrder(i, visited, Stack, adj);  // Perform DFS and fill the stack
        }
    }

    unordered_map<int, list<int>> transposedAdj = getTranspose(n, adj);  // Get the transposed graph

    fill(visited.begin(), visited.end(), false);  // Mark all vertices as not visited for the second DFS

    int sccCount = 1;  // Counter for SCCs

    while (!Stack.empty()) {  // Process all vertices in the order defined by the stack
        int v = Stack.top();  // Get the top vertex
        Stack.pop();  // Remove the top vertex

        if (!visited[v]) {  // If the vertex has not been visited
            vector<int> component;  // To store the current strongly connected component
            DFSUtil(v, visited, transposedAdj, component);  // Perform DFS on the transposed graph
            cout << "SCC " << sccCount << " is: ";
            for (int vertex : component) {  // Iterate over all vertices in the component
                cout << vertex << " ";  // Print each vertex in the component
            }
            cout << endl;  // Newline for the next component
            sccCount++;  // Increment the counter
        }
    }
}

int main() {
    int n, m;
    
    // Prompt the user for input
    cout << "Enter the number of vertices: ";
    cin >> n;
    cout << "Enter the number of edges: ";
    cin >> m;
    
    vector<pair<int, int>> edges(m);  // Vector to store all the edges
    cout << "Enter the edges (u v):" << endl;
    for (int i = 0; i < m; i++) {  // Read each edge
        cin >> edges[i].first >> edges[i].second;
    }

    findSCCs(n, m, edges);  // Find and print all strongly connected components

    return 0;
}
