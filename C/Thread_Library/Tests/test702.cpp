#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

#define WORK 150
#define NUM_CHILDREN 5

mutex m;
cv c;

bool proceed = false;

void child(void *arg) {
    m.lock();
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

    for (volatile size_t i = 0; i < WORK; [](auto &v) {
             auto t = v;
             v = t + 1;
         }(i)) {
        c.broadcast();
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