#include "reactor.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <algorithm>
#include <chrono>
#include <thread>

using namespace std;

// Global variables to store the graph
vector<list<int>> adj; // Adjacency list for the graph
vector<list<int>> transposedAdj; // Transposed adjacency list for the graph
int n, m;  // Number of vertices and edges

// Function to perform DFS and fill the stack
void fillOrder(int v, vector<bool>& visited, stack<int>& Stack) {
    visited[v] = true;  // Mark the current node as visited
    for (int neighbor : adj[v]) {  // Iterate over all the adjacent vertices
        if (!visited[neighbor]) {  // If the adjacent vertex has not been visited
            fillOrder(neighbor, visited, Stack);  // Recursive call for the adjacent vertex
        }
    }
    Stack.push(v);  // Push the vertex to the stack after all its adjacent vertices are processed
}

// Function to perform DFS on the transposed graph
void DFSUtil(int v, vector<bool>& visited, vector<int>& component) {
    visited[v] = true;  // Mark the current node as visited
    component.push_back(v);  // Add this vertex to the current component
    for (int neighbor : transposedAdj[v]) {  // Iterate over all the adjacent vertices in the transposed graph
        if (!visited[neighbor]) {  // If the adjacent vertex has not been visited
            DFSUtil(neighbor, visited, component);  // Recursive call for the adjacent vertex
        }
    }
}

// Function to get the transposed graph
void getTranspose() {
    transposedAdj = vector<list<int>>(n);  // Create a transposed adjacency list with n vertices
    for (int v = 0; v < n; ++v) {  // Iterate over each vertex
        for (int neighbor : adj[v]) {  // Iterate over all the adjacent vertices
            transposedAdj[neighbor].push_back(v);  // Add an edge from neighbor to v in the transposed graph
        }
    }
}

// Function to find and return all strongly connected components (SCCs)
string findSCCs() {
    stack<int> Stack;  // Stack to store the order of vertices by finishing times
    vector<bool> visited(n, false);  // Visited array to keep track of visited vertices

    // Perform DFS to fill the stack with vertices in order of finishing times
    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            fillOrder(i, visited, Stack);
        }
    }

    getTranspose();  // Get the transposed graph

    fill(visited.begin(), visited.end(), false);  // Mark all vertices as not visited for the second DFS

    stringstream ss;  // String stream to store the SCCs result
    int scc_count = 0;  // Counter for SCCs

    // Process all vertices in order defined by the stack
    while (!Stack.empty()) {
        int v = Stack.top();
        Stack.pop();

        if (!visited[v]) {
            vector<int> component;  // Vector to store the current SCC
            DFSUtil(v, visited, component);  // Perform DFS on the transposed graph
            ss << "SCC " << ++scc_count << " is: ";
            for (int vertex : component) {
                ss << (vertex + 1) << " ";  // Add vertices to the SCC result
            }
            ss << endl;
        }
    }

    return ss.str();  // Return the result string
}

// Function to handle the "Newgraph" command
void handleNewGraph(int vertices, int edges, int client_fd) {
    n = vertices;  // Set the number of vertices
    m = edges;  // Set the number of edges
    adj = vector<list<int>>(n);  // Initialize the adjacency list with n vertices
    char buf[256];  // Buffer to store received data
    int u, v;

    for (int i = 0; i < m; ++i) {
        memset(buf, 0, sizeof(buf));  // Clear the buffer
        int nbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);  // Receive data from the client
        if (nbytes <= 0) {
            if (nbytes == 0) {
                cout << "Socket " << client_fd << " hung up" << endl;
            } else {
                perror("recv");
            }
            close(client_fd);  // Close the client socket
            return;
        }
        buf[nbytes] = '\0';  // Null-terminate the buffer
        stringstream ss(buf);  // Create a string stream from the buffer
        ss >> u >> v;  // Parse the edge endpoints
        if (u < 1 || u > n || v < 1 || v > n) {
            cerr << "Invalid edge: " << u << " " << v << endl;
            --i; // Retry the current edge
            continue;
        }
        adj[u - 1].push_back(v - 1);  // Add the edge to the adjacency list
    }
    cout << "Graph with " << n << " vertices and " << m << " edges created." << endl;
}

// Function to handle the "Newedge" command
void handleNewEdge(int u, int v) {
    if (u < 1 || u > n || v < 1 || v > n) {
        cerr << "Invalid edge: " << u << " " << v << endl;
        return;
    }
    adj[u - 1].push_back(v - 1);  // Add the edge to the adjacency list
    cout << "Edge added: " << u << " -> " << v << endl;
}

// Function to handle the "Removeedge" command
void handleRemoveEdge(int u, int v) {
    if (u < 1 || u > n || v < 1 || v > n) {
        cerr << "Invalid edge: " << u << " " << v << endl;
        return;
    }
    adj[u - 1].remove(v - 1);  // Remove the edge from the adjacency list
    cout << "Edge removed: " << u << " -> " << v << endl;
}

// Function to convert a string to lowercase
string toLowerCase(const string& str) {
    string result = str;  // Create a copy of the string
    transform(result.begin(), result.end(), result.begin(), ::tolower);  // Convert all characters to lowercase
    return result;  // Return the lowercase string
}

// Function to handle client commands
void handleClient(int client_fd) {
    char buf[1024];  // Buffer to store received data
    int nbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);  // Receive data from the client
    if (nbytes <= 0) {
        if (nbytes == 0) {
            cout << "Socket " << client_fd << " hung up" << endl;
        } else {
            perror("recv");
        }
        close(client_fd);  // Close the client socket
        return;
    }
    buf[nbytes] = '\0';  // Null-terminate the buffer
    string command(buf);  // Convert the buffer to a string
    stringstream ss(command);  // Create a string stream from the command
    string cmd;  // String to store the parsed command
    ss >> cmd;  // Parse the command
    cmd = toLowerCase(cmd);  // Convert command to lowercase
    string response;  // String to store the response
    if (cmd == "newgraph") {
        int vertices, edges;
        ss >> vertices >> edges;  // Parse the number of vertices and edges
        response = "Send the edges.\n";
        send(client_fd, response.c_str(), response.length(), 0);  // Send the response to the client
        handleNewGraph(vertices, edges, client_fd);  // Handle the Newgraph command
        response = "New graph created.\n";
        send(client_fd, response.c_str(), response.length(), 0);  // Send the response to the client
    } else if (cmd == "kosaraju") {
        response = findSCCs();  // Find the SCCs
        send(client_fd, response.c_str(), response.length(), 0);  // Send the response to the client
    } else if (cmd == "newedge") {
        int u, v;
        ss >> u >> v;  // Parse the edge endpoints
        handleNewEdge(u, v);  // Handle the Newedge command
        response = "Edge added.\n";
        send(client_fd, response.c_str(), response.length(), 0);  // Send the response to the client
    } else if (cmd == "removeedge") {
        int u, v;
        ss >> u >> v;  // Parse the edge endpoints
        handleRemoveEdge(u, v);  // Handle the Removeedge command
        response = "Edge removed.\n";
        send(client_fd, response.c_str(), response.length(), 0);  // Send the response to the client
    } else {
        response = "Invalid command.\n";
        send(client_fd, response.c_str(), response.length(), 0);  // Send an error message for invalid commands
    }
}

// Main function
int main() {
    int listener;  // Listening socket descriptor
    struct sockaddr_in myaddr;  // Server address
    int yes = 1;  // For setsockopt() SO_REUSEADDR, below
    int port = 9034;  // Port number

    // Create a socket
    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Lose the pesky "Address already in use" error message
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    myaddr.sin_family = AF_INET;  // Use IPv4
    myaddr.sin_addr.s_addr = INADDR_ANY;  // Bind to any available interface
    myaddr.sin_port = htons(port);  // Set the port number
    memset(&(myaddr.sin_zero), '\0', 8);  // Zero the rest of the struct

    // Bind the socket to the address and port
    if (bind(listener, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    // Listen for incoming connections
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(1);
    }

    Reactor reactor;  // Create a reactor
    reactor.startReactor();  // Start the reactor
    reactor.addFdToReactor(listener, [&](int fd) {
        struct sockaddr_in remoteaddr;  // Client address
        socklen_t addrlen = sizeof(remoteaddr);
        int newfd = accept(fd, (struct sockaddr *)&remoteaddr, &addrlen);  // Accept a new connection
        if (newfd == -1) {
            perror("accept");
        } else {
            cout << "New connection from "
                 << inet_ntoa(remoteaddr.sin_addr) << " on socket " << newfd << endl;
            reactor.addFdToReactor(newfd, handleClient);  // Add the new client to the reactor
        }
    });

    // Keep the server running indefinitely
    while (true) {
        std::this_thread::sleep_for(std::chrono::minutes(10));  // Sleep to keep the main thread running
    }

    reactor.stopReactor();  // Stop the reactor (unreachable code in this case)
    close(listener);  // Close the listening socket
    return 0;
}