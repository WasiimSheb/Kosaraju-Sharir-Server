#include "proactor.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Helper structure to pass arguments to the run function
struct ProactorArgs {
    int sockfd;
    proactorFunc func;
};

// Function to run the proactor thread
void* Proactor::run(void* arg) {
    ProactorArgs* args = static_cast<ProactorArgs*>(arg);
    int sockfd = args->sockfd;
    proactorFunc func = args->func;

    // Call the provided function with the socket file descriptor
    func(sockfd);

    // Clean up and exit the thread
    close(sockfd);
    delete args;
    return nullptr;
}

// Starts a new proactor and returns the proactor thread id
pthread_t Proactor::startProactor(int sockfd, proactorFunc threadFunc) {
    pthread_t tid;
    ProactorArgs* args = new ProactorArgs{sockfd, threadFunc};

    // Create a new thread to handle the client connection
    if (pthread_create(&tid, nullptr, run, args) != 0) {
        perror("pthread_create");
        delete args;
        return 0;
    }

    // Store the thread function in the map
    {
        std::lock_guard<std::mutex> lock(map_mutex);
        threadFuncs[tid] = threadFunc;
    }

    return tid;
}

// Stops the proactor by thread id
int Proactor::stopProactor(pthread_t tid) {
    // Attempt to cancel the thread
    if (pthread_cancel(tid) != 0) {
        perror("pthread_cancel");
        return -1;
    }

    // Remove the thread function from the map
    {
        std::lock_guard<std::mutex> lock(map_mutex);
        threadFuncs.erase(tid);
    }

    return 0;
}
