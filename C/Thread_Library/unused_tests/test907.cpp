#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

#define WORK 10000000
#define NUM_CHILD_1 1

// CV TEST 210, signaling a waiiting thread but not releasing the mutex for that thread -> should be no problem

mutex m1, m2;
cv c1, c2;

bool child1finished = false;
bool child0finished = false;

int critical_data = 0;

void child0(void *arg) {
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] is acquiring lock m1\n";
    assert_interrupts_enabled();
    m1.lock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] acquires lock m1 successfully\n";
    assert_interrupts_enabled();

    while (!child1finished) { // Not waiting in while
        c1.wait(m1);
    }

    std::cout << "[Child " << *((size_t*) arg) << "] Got woken up\n";

    assert_interrupts_enabled();

    child0finished = true;

    c2.signal();

    assert_interrupts_enabled();

    m1.unlock();

    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] finished, deleting then returning\n";
    delete (size_t*)arg;
}

void child1(void *arg) {
    assert_interrupts_enabled();
    thread::yield(); // Give CPU to child 0 in case child 1 runs first

    assert_interrupts_enabled();

    std::cout << "[Child " << *((size_t*) arg) << "] is acquiring lock m2\n";
    m2.lock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] acquired lock m2 successfully\n";
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] about to do work\n";
    assert_interrupts_enabled();
    
    for (volatile size_t i = 0; i < WORK; [](auto& v) { auto t = v; v = t + 1; } (i)) {
        if (!(i%(WORK/2))) {
            std::cout << "[Child " << *((size_t*) arg) << "] Blehh\n";
            assert_interrupts_enabled();
        }
    }

    std::cout << "[Child " << *((size_t*) arg) << "] completed work, signaling child 0\n";
    c1.signal();
    assert_interrupts_enabled();

    child1finished = true;

    assert_interrupts_enabled();

    m2.unlock();

    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] unlocked m2 sucessfully, deleting then returning\n";
    delete (size_t*)arg;
}

void parent(void *arg) {
    assert_interrupts_enabled();
    m1.lock();
    assert_interrupts_enabled();
    std::cout << "Parent locked m1 and spawning children\n";

    thread t0(child0, (void*)new size_t(0));


    for (size_t i=0; i < NUM_CHILD_1; ++i) {
        thread t(child1, (void*)new size_t(1+i));
    }

    assert_interrupts_enabled();

    thread::yield();

    assert_interrupts_enabled();

    thread::yield();

    assert_interrupts_enabled();

    while (!child0finished) {
        c2.wait(m1);
    }

    m1.unlock();

    assert_interrupts_enabled();

    std::cout << "Parent returning\n";
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), false,
              false, 1);
    return 0;
}