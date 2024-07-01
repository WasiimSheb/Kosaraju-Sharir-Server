#include <iostream>
#include <vector>
#include <stack>
#include <list>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

using namespace std;

#define PORT "9034"   // the port users will be connecting to

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

    // Method to compute and return all SCCs using Kosaraju's algorithm
    string kosaraju() {
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

        // Prepare the SCCs result string
        stringstream ss;
        ss << "Total number of SCCs: " << sccs.size() << endl;
        for (int i = 0; i < sccs.size(); ++i) {
            ss << "SCC " << (i + 1) << " is: ";
            for (int vertex : sccs[i]) {
                ss << (vertex + 1) << " ";
            }
            ss << endl;
        }
        return ss.str();
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
void handleNewGraph(Graph*& graph, int n, int m, int client_fd) {
    delete graph;  // Delete the old graph if it exists
    graph = new Graph(n);  // Create a new graph with n vertices
    char buffer[256];
    int u, v;
    for (int i = 0; i < m; ++i) {  // Read m edges
        int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            perror("recv");
            return;
        }
        buffer[bytes_received] = '\0';
        sscanf(buffer, "%d %d", &u, &v);
        graph->addEdge(u, v);
    }
    send(client_fd, "Graph created.\n", 15, 0);
}

// Function to handle the "Newedge" command
void handleNewEdge(Graph*& graph, int u, int v, int client_fd) {
    graph->addEdge(u, v);
    send(client_fd, "Edge added.\n", 12, 0);
}

// Function to handle the "Removeedge" command
void handleRemoveEdge(Graph*& graph, int u, int v, int client_fd) {
    graph->removeEdge(u, v);
    send(client_fd, "Edge removed.\n", 14, 0);
}

// Function to handle the "Kosaraju" command
void handleKosaraju(Graph*& graph, int client_fd) {
    if (graph != nullptr) {
        string result = graph->kosaraju();
        send(client_fd, result.c_str(), result.size(), 0);
    } else {
        send(client_fd, "No graph available. Use 'Newgraph' command to create a graph first.\n", 67, 0);
    }
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void) {
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    Graph* graph = nullptr;

    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("selectserver: new connection from %s on "
                            "socket %d\n",
                            inet_ntop(remoteaddr.ss_family,
                                get_in_addr((struct sockaddr*)&remoteaddr),
                                remoteIP, INET6_ADDRSTRLEN),
                            newfd);
                    }
                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {
                        // we got some data from a client
                        buf[nbytes] = '\0';
                        string command(buf);
                        istringstream iss(command);
                        string cmd;
                        iss >> cmd;

                        if (cmd == "Newgraph") {
                            int n, m;
                            iss >> n >> m;
                            handleNewGraph(graph, n, m, i);
                        } else if (cmd == "Kosaraju") {
                            handleKosaraju(graph, i);
                        } else if (cmd == "Newedge") {
                            int u, v;
                            iss >> u >> v;
                            handleNewEdge(graph, u, v, i);
                        } else if (cmd == "Removeedge") {
                            int u, v;
                            iss >> u >> v;
                            handleRemoveEdge(graph, u, v, i);
                        } else {
                            const char *msg = "Invalid command.\n";
                            send(i, msg, strlen(msg), 0);
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)

    return 0;
}
