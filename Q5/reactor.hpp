#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <sys/select.h>
#include <map>
#include <functional>
#include <unistd.h>

// Type definition for the reactor function callback
typedef std::function<void(int)> reactorFunc;

// Reactor class definition
class Reactor {
public:
    Reactor();  // Constructor
    ~Reactor();  // Destructor

    // Starts the reactor and returns a pointer to it
    void* startReactor();

    // Adds a file descriptor to the reactor with the specified callback function
    int addFdToReactor(int fd, reactorFunc func);

    // Removes a file descriptor from the reactor
    int removeFdFromReactor(int fd);

    // Stops the reactor
    int stopReactor();

private:
    fd_set masterSet;  // Master set of file descriptors
    fd_set readSet;    // Temporary set of file descriptors for select()
    int fdMax;         // Maximum file descriptor number
    bool running;      // Flag indicating if the reactor is running
    std::map<int, reactorFunc> callbacks;  // Map of file descriptors to their callback functions

    // Main loop of the reactor
    void run();
};

#endif // REACTOR_HPP
