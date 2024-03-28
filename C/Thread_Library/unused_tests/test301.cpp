#include <deque>
#include <iostream>
#include <cassert>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

#define NUM_THREADS 1 // Number of Child Threads
#define WORK 10000000

// Join test 301 -> One thread joining itself -> should not be a problem, but will result in deadlock

thread* myself = nullptr;

void child(void *arg) {
    assert_interrupts_enabled();
    assert(myself != nullptr); // Make sure myself pointer is set

    std::cout << "[Child " << *((size_t*) arg) << "] begins and tries to join itself\n";
    assert_interrupts_enabled();
    myself->join();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] returns from join and does work\n";

    for (volatile size_t i = 0; i < WORK; [](auto& v) { auto t = v; v = t + 1; } (i)) { 
        if (!(i%(WORK/2))) {
            std::cout << "[Child " << *((size_t*) arg) << "] Bleeehhh\n";
            assert_interrupts_enabled();
        }
    }

    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] deleting then returning\n";
    delete (size_t*)arg;
}

void parent(void *arg) {
    assert_interrupts_enabled();

    std::cout << "Parent spawning children\n";

    for (size_t i = 0; i < NUM_THREADS; ++i) {
        thread t(child, (void*)new size_t(i));
        myself = &t;
        thread::yield(); // Force to switch to child thread
        assert_interrupts_enabled();
    }

    assert_interrupts_enabled();
    std::cout << "Parent returning\n";
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), true,
              true, 1);
    return 0;
}