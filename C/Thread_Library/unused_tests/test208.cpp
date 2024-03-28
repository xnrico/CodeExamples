#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

#define WORK 10000000
#define NUM_CHILD_1 1

// CV TEST 208, waiting WITH checking condition inside while loop -> should be correct

mutex m;
cv c;

bool child1finished = false;

int critical_data = 0;

void child0(void *arg) {
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] is acquiring lock m\n";
    assert_interrupts_enabled();
    m.lock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] acquires lock m\n";
    assert_interrupts_enabled();

    while (!child1finished) { // Not waiting in while
        c.wait(m);
    }

    critical_data = -999999; // Make it Negative should not run until child1 finishes

    assert_interrupts_enabled();
    m.unlock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] made critical data negative and unlocked, deleting then returning\n";
    delete (size_t*)arg;
}

void child1(void *arg) {
    assert_interrupts_enabled();
    thread::yield(); // Give CPU to child 0 in case child 1 runs first

    assert_interrupts_enabled();

    c.signal(); // Signals on purpose then yield

    assert_interrupts_enabled();

    thread::yield();

    assert_interrupts_enabled();

    std::cout << "[Child " << *((size_t*) arg) << "] is acquiring lock m\n";
    m.lock();
    std::cout << "[Child " << *((size_t*) arg) << "] acquired lock m successfully\n";
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] about to do work\n";
    assert_interrupts_enabled();
    
    for (volatile size_t i = 0; i < WORK; [](auto& v) { auto t = v; v = t + 1; } (i)) {
        if (!(i%(WORK/2))) {
            critical_data  += 5;
            std::cout << "[Child " << *((size_t*) arg) << "] Critical Data SHOULD NOT BE NEGATIVE and is now: " << critical_data << "\n";
            assert_interrupts_enabled();
        }
    }

    std::cout << "[Child " << *((size_t*) arg) << "] completed work, signaling child 0\n";
    c.signal();
    child1finished = true;
    m.unlock();
    std::cout << "[Child " << *((size_t*) arg) << "] finished and unlocked sucessfully, deleting then returning\n";
    delete (size_t*)arg;
}

void parent(void *arg) {
    assert_interrupts_enabled();

    std::cout << "Parent spawning children\n";

    thread t0(child0, (void*)new size_t(0));

    for (size_t i=0; i < NUM_CHILD_1; ++i) {
        thread t(child1, (void*)new size_t(1+i));
    }

    assert_interrupts_enabled();

    std::cout << "Parent returning\n";
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), true,
              true, 1);
    return 0;
}