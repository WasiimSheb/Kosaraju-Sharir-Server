// kosaraju_interactive.cpp
// This file implements Kosaraju's algorithm for finding strongly connected components (SCCs) in a directed graph
// using interactive commands via stdin. The program can create a new graph, add or remove edges, and compute SCCs.

#include <iostream>
#include <vector>
#include <stack>
#include <list>
#include <sstream>

using namespace std;

// Class to manage the graph and its operations
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

    // Method to compute and print all SCCs using Kosaraju's algorithm
    void kosaraju() {
        vector<vector<int>> sccs;  // Vector to store all SCCs
        vector<bool> visited(n, false);  // Visited array to keep track of visited vertices
        stack<int> Stack;  // Stack to store the order of vertices by finishing times

        // Perform DFS for each vertex to fill the stack
        for (int i = 0; i < n; ++i) {
            if (!visited[i]) {
                fillOrder(i, visited, Stack);
            }
        }

        list<int>* transposedAdj = getTranspose();  // Get the transposed graph

        fill(visited.begin(), visited.end(), false);  // Mark all vertices as not visited for the second DFS

        // Process all vertices in the order defined by the stack
        while (!Stack.empty()) {
            int v = Stack.top();
            Stack.pop();

            if (!visited[v]) {
                vector<int> component;  // Vector to store the current SCC
                DFSUtil(v, visited, transposedAdj, component);  // Perform DFS on the transposed graph
                sccs.push_back(component);
            }
        }

        delete[] transposedAdj;  // Free the memory used by the transposed graph

        // Print the SCCs
        cout << "Total number of SCCs: " << sccs.size() << endl;
        for (int i = 0; i < sccs.size(); ++i) {
            cout << "SCC " << (i + 1) << " is: ";
            for (int vertex : sccs[i]) {
                cout << (vertex + 1) << " ";
            }
            cout << endl;
        }
    }

private:
    vector<list<int>> adj;  // Adjacency list representation of the graph
    int n;  // Number of vertices in the graph

    // Helper method to perform DFS and fill the stack
    void fillOrder(int v, vector<bool>& visited, stack<int>& Stack) {
        visited[v] = true;  // Mark the current node as visited
        for (int neighbor : adj[v]) {  // Iterate over all the adjacent vertices
            if (!visited[neighbor]) {  // If the adjacent vertex has not been visited
                fillOrder(neighbor, visited, Stack);  // Recursive call for the adjacent vertex
            }
        }
        Stack.push(v);  // Push the vertex to the stack after all its adjacent vertices are processed
    }

    // Helper method to perform DFS on the transposed graph
    void DFSUtil(int v, vector<bool>& visited, list<int>* transposedAdj, vector<int>& component) {
        visited[v] = true;  // Mark the current node as visited
        component.push_back(v);  // Add this vertex to the current component
        for (int neighbor : transposedAdj[v]) {  // Iterate over all the adjacent vertices in the transposed graph
            if (!visited[neighbor]) {  // If the adjacent vertex has not been visited
                DFSUtil(neighbor, visited, transposedAdj, component);  // Recursive call for the adjacent vertex
            }
        }
    }

    // Helper method to get the transposed graph
    list<int>* getTranspose() {
        list<int>* transposedAdj = new list<int>[n];  // Create an empty transposed graph
        for (int v = 0; v < n; ++v) {  // Iterate over each vertex
            for (int neighbor : adj[v]) {  // Iterate over all the adjacent vertices
                transposedAdj[neighbor].push_back(v);  // Add an edge from neighbor to v in the transposed graph
            }
        }
        return transposedAdj;  // Return the transposed graph
    }
};

// Function to handle the "Newgraph" command
void handleNewGraph(Graph*& graph, int n, int m) {
    delete graph;  // Delete the old graph if it exists
    graph = new Graph(n);  // Create a new graph with n vertices
    int u, v;
    cout << "Enter " << m << " edges (u v):" << endl;
    for (int i = 0; i < m; ++i) {  // Read m edges
        cin >> u >> v;
        graph->addEdge(u, v);
    }
    cin.ignore();  // Clear the newline character left in the input buffer
}

// Function to handle the "Newedge" command
void handleNewEdge(Graph*& graph, int u, int v) {
    graph->addEdge(u, v);
}

// Function to handle the "Removeedge" command
void handleRemoveEdge(Graph*& graph, int u, int v) {
    graph->removeEdge(u, v);
}

int main() {
    Graph* graph = nullptr;  // Pointer to the graph
    string command;

    while (true) {
        cout << "Enter command (Newgraph, Kosaraju, Newedge, Removeedge) or 'exit' to quit:" << endl;
        getline(cin, command);
        istringstream iss(command);
        string cmd;
        iss >> cmd;

        if (cmd == "Newgraph") {  // Handle "Newgraph" command
            int n, m;
            iss >> n >> m;
            handleNewGraph(graph, n, m);
        } else if (cmd == "Kosaraju") {  // Handle "Kosaraju" command
            if (graph != nullptr) {
                graph->kosaraju();
            } else {
                cerr << "No graph available. Use 'Newgraph' command to create a graph first." << endl;
            }
        } else if (cmd == "Newedge") {  // Handle "Newedge" command
            int u, v;
            iss >> u >> v;
            handleNewEdge(graph, u, v);
        } else if (cmd == "Removeedge") {  // Handle "Removeedge" command
            int u, v;
            iss >> u >> v;
            handleRemoveEdge(graph, u, v);
        } else if (cmd == "exit") {  // Exit the program
            break;
        } else {
            cerr << "Invalid command." << endl;
        }
    }

    delete graph;  // Free the memory used by the graph
    return 0;
}
