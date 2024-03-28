#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

#define WORK 10000000

// CV TEST 201, Signal

mutex m;
cv c;

bool child1finished = false;

void child0(void *arg) {
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] is acquiring lock m\n";
    m.lock();
    std::cout << "[Child " << *((size_t*) arg) << "] acquired lock m successfully, waiting for child 1 to finish\n";
    assert_interrupts_enabled();

    while (!child1finished) {
        c.wait(m);
    }

    std::cout << "[Child " << *((size_t*) arg) << "] woke up from wait() to proceed\n";

    assert_interrupts_enabled();
    
    for (volatile size_t i = 0; i < WORK; [](auto& v) { auto t = v; v = t + 1; } (i)) {
        if (!(i%(WORK/2))) {
            std::cout << "[Child " << *((size_t*) arg) << "] Bleeehhh\n";
            assert_interrupts_enabled();
        }
    }

    m.unlock();
    std::cout << "[Child " << *((size_t*) arg) << "] unlocked sucessfully, deleting then returning\n";
    delete (size_t*)arg;
}

void child1(void *arg) {
    assert_interrupts_enabled();
    thread::yield(); // Give CPU back to child 0 in case child 1 runs first
    assert_interrupts_enabled();

    std::cout << "[Child " << *((size_t*) arg) << "] is acquiring lock m\n";
    m.lock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] acquired lock m successfully, doing work\n";

    for (volatile size_t i = 0; i < WORK; [](auto& v) { auto t = v; v = t + 1; } (i)) {
        if (!(i%(WORK/2))) {
            std::cout << "[Child " << *((size_t*) arg) << "] Bleeehhh\n";
            assert_interrupts_enabled();
        }
    }

    child1finished = true;

    c.signal();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] completed work and signaled child 0\n";

    m.unlock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] unlocked sucessfully, deleting then returning\n";
    delete (size_t*)arg;
}

void parent(void *arg) {
    assert_interrupts_enabled();

    std::cout << "Parent spawning children\n";

    thread t0(child0, (void*)new size_t(0));
    thread t1(child1, (void*)new size_t(1));

    assert_interrupts_enabled();

    std::cout << "Parent returning\n";
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), true,
              true, 1);
    return 0;
}