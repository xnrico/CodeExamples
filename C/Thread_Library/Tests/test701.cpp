#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

#define WORK 100
#define NUM_CHILDREN 5

mutex m;
cv c;

bool proceed = false;

void child(void *arg) {
    m.lock();
    if (!proceed) {
        c.wait(m);
    }

    for (volatile size_t i = 0; i < WORK; [](auto &v) {
             auto t = v;
             v = t + 1;
         }(i)) {
        assert_interrupts_enabled();
    }

    m.unlock();
    delete (size_t *)arg;
}

void parent(void *arg) {
    thread *children[NUM_CHILDREN];

    for (size_t i = 0; i < NUM_CHILDREN; ++i) {
        children[i] = new thread(child, (void *)new size_t(i));
        std::cout << "Spawned and destroyed a child object\n";
        assert_interrupts_enabled();
    }

    thread::yield();
    thread::yield();
    thread::yield();
    thread::yield();

    proceed = true;
    c.signal();

    for (size_t i = 0; i < NUM_CHILDREN; ++i) {
        children[i]->join();  // Children should have been destroyed
        std::cout << "One child joined\n";
        assert_interrupts_enabled();
    }
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), false,
              false, 1);
    return 0;
}