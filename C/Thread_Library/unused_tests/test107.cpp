#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

#define WORK 10000000

bool child0_locked_m1 = false;
bool child1_locked_m2 = false;

// MUTEX TEST 107, if WaitingQueue is static, this should be a problem

mutex m1;
mutex m2;

void child0(void *arg) {
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] is acquiring lock m1\n";
    m1.lock();

    assert_interrupts_enabled();
    child0_locked_m1 = true;
    std::cout << "[Child " << *((size_t*) arg) << "] acquired m1 successfully\n";

    thread::yield(); // GO to the other child thread

    
    for (volatile size_t i = 0; i < WORK; [](auto& v) { auto t = v; v = t + 1; } (i)) {
        if (!(i%(WORK/2))) {
            std::cout << "[Child " << *((size_t*) arg) << "] Bleeehhh\n";
            assert_interrupts_enabled();
        }
    }

    std::cout << "[Child " << *((size_t*) arg) << "] completed work then unlocking m1\n";
    assert_interrupts_enabled();
    m1.unlock();

    assert_interrupts_enabled();

    std::cout << "[Child " << *((size_t*) arg) << "] unlocked m1 sucessfully, deleting then returning\n";
    delete (size_t*)arg;
}

void child1(void *arg) {
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] is acquiring lock m2\n";
    m2.lock();

    assert_interrupts_enabled();
    child1_locked_m2 = true;
    std::cout << "[Child " << *((size_t*) arg) << "] acquired m2 successfully\n";

    thread::yield();
    
    for (volatile size_t i = 0; i < WORK; [](auto& v) { auto t = v; v = t + 1; } (i)) {
        if (!(i%(WORK/2))) {
            std::cout << "[Child " << *((size_t*) arg) << "] Bleeehhh\n";
            assert_interrupts_enabled();
        }
    }

    std::cout << "[Child " << *((size_t*) arg) << "] completed work then unlocking m2\n";
    assert_interrupts_enabled();
    m2.unlock();

    assert_interrupts_enabled();
    
    std::cout << "[Child " << *((size_t*) arg) << "] unlocked m2 sucessfully, deleting then returning\n";
    delete (size_t*)arg;
}

void parent(void *arg) {
    assert_interrupts_enabled();

    m1.lock();
    m2.lock();
    std::cout << "Parent has lock m1 & m2 and spawning children\n";

    thread t0(child0, (void*)new size_t(0));
    thread t1(child1, (void*)new size_t(1));
    thread::yield(); // Go to child

    assert_interrupts_enabled();

    std::cout << "Parent about to unlock m1\n";
    m1.unlock();
    assert_interrupts_enabled();
    thread::yield(); // Go back to child, now if queue is static, child 1 with m2 will do work instead of child1 in certain interleavings

    assert_interrupts_enabled();

    std::cout << "Parent returning\n";
    // Also returning without unlocking m2 -> but this should be fine
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), true,
              true, 1);
    return 0;
}