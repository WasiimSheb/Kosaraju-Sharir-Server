#include "kosaraju_server.hpp"
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
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

#define PORT "9034"   // the port users will be connecting to

// Global variables for the monitoring thread
mutex graph_mutex;
condition_variable scc_cond;
bool scc_condition_met = false;
bool notify = false;

void monitoringThread() {
    unique_lock<mutex> lock(graph_mutex);
    while (true) {
        scc_cond.wait(lock, [] { return notify; });
        notify = false;
        if (scc_condition_met) {
            cout << "At least 50% of the graph belongs to the same SCC\n";
        } else {
            cout << "At least 50% of the graph no longer belongs to the same SCC\n";
        }
    }
}

// Function to handle the "Newgraph" command
void handleNewGraph(Graph*& graph, int n, int m, int client_fd) {
    lock_guard<mutex> lock(graph_mutex);
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
    lock_guard<mutex> lock(graph_mutex);
    graph->addEdge(u, v);
    send(client_fd, "Edge added.\n", 12, 0);
    notify = true;
    scc_cond.notify_one();
}

// Function to handle the "Removeedge" command
void handleRemoveEdge(Graph*& graph, int u, int v, int client_fd) {
    lock_guard<mutex> lock(graph_mutex);
    graph->removeEdge(u, v);
    send(client_fd, "Edge removed.\n", 14, 0);
    notify = true;
    scc_cond.notify_one();
}

// Function to handle the "Kosaraju" command
void handleKosaraju(Graph*& graph, int client_fd) {
    if (graph != nullptr) {
        bool condition_met;
        string result = graph->kosaraju(condition_met);
        {
            lock_guard<mutex> lock(graph_mutex);
            if (condition_met != scc_condition_met) {
                scc_condition_met = condition_met;
                notify = true;
                scc_cond.notify_one();
            }
        }
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

    // start the monitoring thread
    thread monitor(monitoringThread);

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

    monitor.join();
    return 0;
}
