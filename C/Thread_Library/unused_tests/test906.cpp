#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

#define WORK 10000
#define NUM_CHILD_1 20

// CV TEST 206, Signal with a many child threads

mutex m;
cv c;

bool child0finished = false;
size_t unfinishedChild1 = NUM_CHILD_1;

void child0(void *arg) {
    assert_interrupts_enabled();
    thread::yield(); // Give CPU back to child 1 in case child 0 runs first
    assert_interrupts_enabled();

    while (unfinishedChild1 != 0) {
        // std::cout << "[Child " << *((size_t*) arg) << "] is acquiring lock m\n";
        m.lock();
        assert_interrupts_enabled();
        // std::cout << "[Child " << *((size_t*) arg) << "] acquired lock m successfully, doing work\n";

        for (volatile size_t i = 0; i < WORK; [](auto& v) { auto t = v; v = t + 1; } (i)) {
            if (!(i%(WORK/2))) {
                // std::cout << "[Child " << *((size_t*) arg) << "] Bleeehhh\n";
                assert_interrupts_enabled();
            }
        }

        child0finished = true;

        c.signal();
        assert_interrupts_enabled();
        std::cout << "[Child " << *((size_t*) arg) << "] completed work and wakes up one waiting child\n";
        m.unlock();
    }
    
    assert_interrupts_enabled();
    std::cout << "ALL CHILDREN FINSIHED! [Child " << *((size_t*) arg) << "] unlocked sucessfully, deleting then returning\n";
    delete (size_t*)arg;
}


void child1(void *arg) {
    assert_interrupts_enabled();
    // std::cout << "[Child " << *((size_t*) arg) << "] is acquiring lock m\n";
    m.lock();
    // std::cout << "[Child " << *((size_t*) arg) << "] acquired lock m successfully, waiting for child 0 to finish\n";
    assert_interrupts_enabled();

    while (!child0finished) {
        c.wait(m);
    }

    child0finished = false;
    // std::cout << "[Child " << *((size_t*) arg) << "] woke up from wait() to proceed\n";

    assert_interrupts_enabled();
    
    for (volatile size_t i = 0; i < WORK; [](auto& v) { auto t = v; v = t + 1; } (i)) {
        if (!(i%(WORK/2))) {
            // std::cout << "[Child " << *((size_t*) arg) << "] Bleeehhh\n";
            assert_interrupts_enabled();
        }
    }

    m.unlock();
    std::cout << "[Child " << *((size_t*) arg) << "] unlocked sucessfully, deleting then returning\n";
    delete (size_t*)arg;
    --unfinishedChild1;
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
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), false,
              false, 1);
    return 0;
}