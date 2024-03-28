#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

constexpr int NUM_CHILDREN = 10;

mutex m;
cv c;

size_t count = 0;

void bad_child(void *arg) {
    m.lock();
    while (true) {
        c.wait(m);
    }

    m.unlock();
}

void child(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 15; ++i) {
        m.lock();
        printf("hi my id is %d\n", id);
        m.unlock();
    }
}

void parent(void *arg) {
    m.lock();
    std::deque<thread> vec;

    for (int i = 0; i < NUM_CHILDREN - 1; ++i) {
        vec.emplace_back(child, (void *)new int(i));
    }

    vec.emplace_back(bad_child, (void *)0);
    m.unlock();

    for (int i = 0; i < NUM_CHILDREN; ++i) {
        vec[i].join();
    }

    printf("All is done!\n");
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), true,
              true, 1);
    return 0;
}