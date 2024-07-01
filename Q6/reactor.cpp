#include "reactor.hpp"
#include <iostream>
#include <thread>

// Constructor initializes the fd sets and variables
Reactor::Reactor() : fdMax(0), running(false) {
    FD_ZERO(&masterSet);  // Initialize the master set to be empty
    FD_ZERO(&readSet);    // Initialize the read set to be empty
}

// Destructor stops the reactor
Reactor::~Reactor() {
    stopReactor();  // Ensure the reactor is stopped when destroyed
}

// Starts the reactor and returns a pointer to it
void* Reactor::startReactor() {
    running = true;  // Set the running flag to true
    std::thread(&Reactor::run, this).detach();  // Run the reactor loop in a separate thread
    return this;  // Return a pointer to the reactor
}

// Adds a file descriptor to the reactor with the specified callback function
int Reactor::addFdToReactor(int fd, reactorFunc func) {
    FD_SET(fd, &masterSet);  // Add the file descriptor to the master set
    if (fd > fdMax) {  // Update the maximum file descriptor if necessary
        fdMax = fd;
    }
    callbacks[fd] = func;  // Store the callback function for the file descriptor
    return 0;  // Return success
}

// Removes a file descriptor from the reactor
int Reactor::removeFdFromReactor(int fd) {
    FD_CLR(fd, &masterSet);  // Remove the file descriptor from the master set
    callbacks.erase(fd);  // Erase the callback function for the file descriptor
    return 0;  // Return success
}

// Stops the reactor
int Reactor::stopReactor() {
    running = false;  // Set the running flag to false
    return 0;  // Return success
}

// Main loop of the reactor
void Reactor::run() {
    while (running) {  // Loop while the reactor is running
        readSet = masterSet;  // Copy the master set to the read set
        int activity = select(fdMax + 1, &readSet, NULL, NULL, NULL);  // Wait for activity on any file descriptor
        if (activity < 0) {  // Check for errors
            perror("select");  // Print an error message
            continue;  // Continue the loop
        }

        for (int i = 0; i <= fdMax; ++i) {  // Loop over all file descriptors
            if (FD_ISSET(i, &readSet)) {  // Check if the file descriptor is ready
                auto it = callbacks.find(i);  // Find the callback function for the file descriptor
                if (it != callbacks.end()) {  // If a callback function is found
                    it->second(i);  // Call the callback function
                }
            }
        }
    }
}
