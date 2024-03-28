#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

constexpr int NUM_CHILDREN = 25;

mutex m;
cv c;

size_t count = 0;

void child(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < 15; ++i) {
        thread::yield();
        m.lock();
        thread::yield();
        ++count;
        thread::yield();
        m.unlock();
        thread::yield();
    }
}

void parent(void *arg) {
    m.lock();
    std::deque<thread> vec;

    for (int i = 0; i < NUM_CHILDREN; ++i) {
        vec.emplace_back(child, (void *)new int(i));
    }

    m.unlock();

    for (int i = 0; i < NUM_CHILDREN; ++i) {
        vec[i].join();
    }

    printf("All is done! Count is: %ld\n", count);
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), true,
              true, 1);
    return 0;
}