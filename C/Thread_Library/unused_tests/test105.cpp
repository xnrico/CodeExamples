#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

#define NUM_THREADS 2 // Number of Child Threads
#define WORK 10000000

// MUTEX TEST 105

mutex m;

void child(void *arg) {
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] is acquiring lock\n";
    m.lock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] acquired lock successfully, doing work\n";

    for (volatile size_t i = 0; i < WORK; [](auto& v) { auto t = v; v = t + 1; } (i)) {
        if (!(i%(WORK/2))) {
            std::cout << "[Child " << *((size_t*) arg) << "] Bleeehhh\n";
            assert_interrupts_enabled();
        }
    }

    std::cout << "[Child " << *((size_t*) arg) << "] completed work, unlocking\n";
    assert_interrupts_enabled();
    m.unlock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] unlocked sucessfully, deleting then returning\n";
    delete (size_t*)arg;
}

void parent(void *arg) {
    assert_interrupts_enabled();

    std::cout << "Parent spawning children\n";

    for (size_t i = 0; i < NUM_THREADS; ++i) {
        thread t(child, (void*)new size_t(i));
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