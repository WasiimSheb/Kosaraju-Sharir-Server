#ifndef KOSARAJU_SERVER_HPP
#define KOSARAJU_SERVER_HPP

#include <vector>
#include <list>
#include <stack>
#include <mutex>
#include <string>

// Global variables for the graph and mutex for thread safety
extern std::vector<std::list<int>> adj; // Adjacency list for the graph
extern std::vector<std::list<int>> transposedAdj; // Transposed adjacency list for the graph
extern int n, m; // Number of vertices and edges in the graph
extern std::mutex graph_mutex; // Mutex to protect the graph data structure

// Function to perform DFS and fill the stack
void fillOrder(int v, std::vector<bool>& visited, std::stack<int>& Stack);

// Function to perform DFS on the transposed graph
void DFSUtil(int v, std::vector<bool>& visited, std::vector<int>& component);

// Function to get the transposed graph
void getTranspose();

// Function to find and return all strongly connected components (SCCs)
std::string findSCCs();

// Function to handle the "Newgraph" command
void handleNewGraph(int vertices, int edges, int client_fd);

// Function to handle the "Newedge" command
void handleNewEdge(int u, int v);

// Function to handle the "Removeedge" command
void handleRemoveEdge(int u, int v);

// Function to convert a string to lowercase
std::string toLowerCase(const std::string& str);

// Function to handle client commands
void handleClient(int client_fd);

#endif // KOSARAJU_SERVER_HPP
