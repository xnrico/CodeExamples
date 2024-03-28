```cpp
#include <iostream>
#include <fstream>
#include <cassert>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include "disk.h"
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"

using std::cout;
using std::endl;

// Define structs for requester and request
typedef struct requester {
    std::vector<int> requests; // Requests made by the requester
    int requester_id; // Unique identifier for the requester
} requester;

typedef struct request {
    int track; // Track number of the request
    int requester_id; // Identifier of the requester making the request
} request;

static std::vector<requester*> requesters; // List of requesters
static std::vector<request*> queue; // Queue of requests
static std::vector<bool> completed; // Status of completion for each requester

// Global variables
static int max_disk_queue = 0;
static int num_requester = 0;
static int total_requests = 0;
static int serviced_requests = 0;
static int num_living_threads = 0;

static mutex mu1; // Mutex for synchronization
static cv cv1, cv2; // Condition variables

// Function to handle servicing of requests
void service_func(void* arg) {
    int current_track = 0;

    while (1) {
        mu1.lock();

        // Check if all requests are serviced
        if (serviced_requests == total_requests && queue.empty()) {
            mu1.unlock();
            return;
        }

        // Wait until conditions are met for serving a request
        while ((num_living_threads > max_disk_queue && (int)queue.size() < max_disk_queue) || (num_living_threads <= max_disk_queue && (int)queue.size() < num_living_threads)) {
            cv1.wait(mu1);
        }

        // Find the request with minimum distance to the current track
        int min_dist = 999999;
        int service_idx = 0;
        for (size_t i=0; i<queue.size(); ++i) { 
            if (fabs(queue[i]->track - current_track) < min_dist) {
                min_dist = fabs(queue[i]->track - current_track);
                service_idx = i;
            }
        }

        // Serve the request
        current_track = queue[service_idx]->track;
        completed[queue[service_idx]->requester_id] = true;
        print_service((unsigned int)queue[service_idx]->requester_id, (unsigned int)queue[service_idx]->track);
        queue.erase(queue.begin() + service_idx);
        ++serviced_requests;

        cv2.broadcast();
        mu1.unlock();
    }
}

// Function to handle requests from a requester
void request_func(void* arg) {
    requester* req = (requester*) arg;

    while (1) {
        mu1.lock();

        // Wait until conditions are met for making a request
        while ((completed[req->requester_id] == false) || 
                (!req->requests.empty() && 
                ((num_living_threads >= max_disk_queue && (int)queue.size() >= max_disk_queue) || (num_living_threads < max_disk_queue && (int)queue.size() >= num_living_threads)))) {
            cv2.wait(mu1);
        }

        // Make a request if requests are pending
        if (req->requests.size() > 0) {
            queue.push_back(new request{req->requests.front(), req->requester_id});
            print_request((unsigned int)req->requester_id, (unsigned int)req->requests.front());
            req->requests.erase(req->requests.begin());
            completed[req->requester_id] = false;

            cv1.signal();
            mu1.unlock();
        }

        // Decrease the count of living threads if no requests are pending
        else {
            --num_living_threads;
            cv1.signal();
            mu1.unlock();
            return;
        }
    }
}

// Function executed by the parent thread
void parent(void* a) {
    thread t0(service_func, (void*)0);

    // Create threads for each requester
    for (int i=0; i < num_requester; ++i) {
        thread t(request_func, (void*)(requesters[i]));
    }
}

int main(int argc, char** argv) {
    assert(argc > 2);
    max_disk_queue = atoi(argv[1]);
    assert(max_disk_queue > 0);

    // Process input files for each requester
    for (int i = argc; i > 2; --i) {
        std::ifstream file(argv[2+num_requester]);
        if (!file) {
            std::cerr << "Error opening file: " << argv[2+num_requester] << std::endl;
            return 1;
        }
        ++num_requester;
        
        std::vector<int> tmp;
        int tmp_num;
        while (file >> tmp_num) {
            tmp.push_back(tmp_num);
            ++total_requests;
        }

        // Create requester and populate requests
        requester* req = new requester{tmp, num_requester-1};
        assert(!tmp.empty());
        requesters.push_back(req);
        completed.push_back(true);
    }

    // Verify the initialization
    assert((int)requesters.size() == num_requester);
    assert((int)completed.size() == num_requester);
    num_living_threads = num_requester;

    // Boot the CPU with the parent thread
    cpu::boot(parent, nullptr, 0);
    return 0;
}
```