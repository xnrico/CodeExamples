#include <cassert>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

#define WORK 100
#define NUM_CHILDREN 5

mutex m;
cv c;

thread *children[NUM_CHILDREN];

bool proceed = false;

void child(void *arg) {
    m.lock();
    assert(arg != nullptr);
    children[(*(size_t *)arg)]->join();
    std::cout << "Child joined target\n";
    m.unlock();
}

void parent(void *arg) {
    m.lock();
    for (size_t i = 0; i < NUM_CHILDREN; ++i) {
        children[i] = new thread(child, (void *)new size_t((i)));
        std::cout << "Spawned and destroyed a child object\n";
        assert_interrupts_enabled();
    }
    m.unlock();

    thread::yield();
    thread::yield();
    thread::yield();
    thread::yield();

    proceed = true;

    for (volatile size_t i = 0; i < WORK; [](auto &v) {
             auto t = v;
             v = t + 1;
         }(i)) {
        c.signal();
    }

    for (size_t i = 0; i < NUM_CHILDREN; ++i) {
        children[i]->join();  // Children should have been destroyed
        std::cout << "One child joined\n";
        assert_interrupts_enabled();
        delete children[i];
    }
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), true,
              true, 1);
    return 0;
}