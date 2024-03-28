// Unlocking unowned mutex!

#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

constexpr long int NUM_CHILDREN = 10;

mutex m;
mutex random_mut;
cv c;

size_t count = 0;

void child(void *arg) {
    size_t id = (size_t)(*((int *)arg));

    for (int i = 0; i < 10; ++i) {
        m.lock();
        ++count;
        m.unlock();
    }

    try {
        m.unlock();
    } catch (std::runtime_error &e) {
        printf("Accidentally unlocked unowned lock! Continuing\n");
    }

    std::cout << "Child " << id << " ended!\n";
}

void parent(void *arg) {
    m.lock();
    std::deque<thread> threads;

    std::cout << "Parent started! Spawning " << NUM_CHILDREN << " children!\n";

    for (size_t i = 0; i < NUM_CHILDREN; ++i) {
        threads.emplace_back(child, (void *)new int(i));
    }

    m.unlock();

    for (size_t i = 0; i < NUM_CHILDREN; ++i) {
        threads[i].join();
    }

    std::cout << "Parent Finished too... Count is " << count << "\n";
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), true,
              true, 1123);
    return 0;
}
