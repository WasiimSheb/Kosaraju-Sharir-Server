#ifndef PROACTOR_HPP
#define PROACTOR_HPP

#include <pthread.h>
#include <functional>
#include <map>
#include <mutex>

// Type definition for the proactor function callback
typedef std::function<void(int)> proactorFunc;

// Proactor class definition
class Proactor {
public:
    // Starts a new proactor and returns the proactor thread id
    pthread_t startProactor(int sockfd, proactorFunc threadFunc);

    // Stops the proactor by thread id
    int stopProactor(pthread_t tid);

private:
    std::map<pthread_t, proactorFunc> threadFuncs; // Map of thread ids to their callback functions
    std::mutex map_mutex; // Mutex to protect access to the map

    // Helper function to run the proactor
    static void* run(void* arg);
};

#endif // PROACTOR_HPP
