#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

#define WORK 10000000
#define NUM_CHILD_1 1

// CV TEST 213, calling the same cv::wait() on different mutexes -> should be no problem

mutex m1, m2;
cv c1;

bool child1proceed = false;
bool child2proceed = false;

int critical_data = 0;

void child0(void *arg) {
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t *)arg) << "] is acquiring lock m1\n";
    assert_interrupts_enabled();
    m1.lock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t *)arg)
              << "] acquired lock m1 successfully, now acquiring lock m2\n";
    assert_interrupts_enabled();
    m2.lock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t *)arg)
              << "] acquired lock m2 successfully, about to do work\n";
    assert_interrupts_enabled();

    for (volatile size_t i = 0; i < WORK; [](auto &v) {
             auto t = v;
             v = t + 1;
         }(i)) {
        if (!(i % (WORK / 2))) {
            std::cout << "[Child " << *((size_t *)arg) << "] Blehh\n";
            assert_interrupts_enabled();
        }
    }

    std::cout << "[Child " << *((size_t *)arg) << "] completed work\n";

    assert_interrupts_enabled();

    child1proceed = true;
    child2proceed = true;

    assert_interrupts_enabled();

    m1.unlock();
    std::cout << "[Child " << *((size_t *)arg) << "] unlocked m1\n";

    m2.unlock();
    std::cout << "[Child " << *((size_t *)arg) << "] unlocked m2\n";

    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t *)arg) << "] finished, deleting then returning\n";
    delete (size_t *)arg;
}

void child1(void *arg) {
    assert_interrupts_enabled();
    thread::yield();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t *)arg) << "] is acquiring lock m1\n";
    m1.lock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t *)arg) << "] acquired lock m1 successfully\n";
    assert_interrupts_enabled();

    if (!child1proceed) {
        c1.wait(m1);
    }

    std::cout << "[Child " << *((size_t *)arg) << "] proceed to work\n";

    assert_interrupts_enabled();
    for (volatile size_t i = 0; i < WORK; [](auto &v) {
             auto t = v;
             v = t + 1;
         }(i)) {
        if (!(i % (WORK / 2))) {
            std::cout << "[Child " << *((size_t *)arg) << "] Blehh\n";
            assert_interrupts_enabled();
        }
    }

    m1.unlock();

    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t *)arg)
              << "] unlocked m1 sucessfully, deleting then returning\n";
    delete (size_t *)arg;
}

void child2(void *arg) {
    assert_interrupts_enabled();
    thread::yield();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t *)arg) << "] is acquiring lock m2\n";
    m2.lock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t *)arg) << "] acquired lock m2 successfully\n";
    assert_interrupts_enabled();

    if (!child2proceed) {
        c1.wait(m2);
    }

    std::cout << "[Child " << *((size_t *)arg) << "] proceed to work\n";

    assert_interrupts_enabled();
    for (volatile size_t i = 0; i < WORK; [](auto &v) {
             auto t = v;
             v = t + 1;
         }(i)) {
        if (!(i % (WORK / 2))) {
            std::cout << "[Child " << *((size_t *)arg) << "] Blehh\n";
            assert_interrupts_enabled();
        }
    }

    m2.unlock();

    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t *)arg)
              << "] unlocked m2 sucessfully, deleting then returning\n";
    delete (size_t *)arg;
}

void parent(void *arg) {
    assert_interrupts_enabled();
    m1.lock();
    assert_interrupts_enabled();
    std::cout << "Parent locked m1 and spawning children\n";

    thread t0(child0, (void *)new size_t(0));

    for (size_t i = 0; i < NUM_CHILD_1; ++i) {
        thread t(child1, (void *)new size_t(1 + i));
    }

    assert_interrupts_enabled();

    thread::yield();

    assert_interrupts_enabled();

    thread::yield();

    assert_interrupts_enabled();

    m1.unlock();

    assert_interrupts_enabled();

    std::cout << "Parent returning\n";
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), false,
              false, 1);
    return 0;
}