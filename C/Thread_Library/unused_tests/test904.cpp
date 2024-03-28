#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

#define WORK 100000
#define NUM_CHILD_1 1

// Mutex TEST 111, philosopher problem

mutex m0, m1, m2, m3, m4;

void child0(void *arg) {
    m0.lock();
    m1.lock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t *)arg)
              << "] acquired lock m0/m1 successfully, about to do work\n";
    assert_interrupts_enabled();

    for (volatile size_t i = 0; i < WORK; [](auto &v) {
             auto t = v;
             v = t + 1;
         }(i)) {
        if (!(i % (WORK / 2))) {
            std::cout << "[Child " << *((size_t *)arg) << "] Working\n";
            assert_interrupts_enabled();
        }
    }

    std::cout << "[Child " << *((size_t *)arg) << "] completed work\n";
    m0.unlock();
    m1.unlock();
    std::cout << "[Child " << *((size_t *)arg) << "] unlocked m0/m1\n";
    assert_interrupts_enabled();
    delete (size_t *)arg;
}

void child1(void *arg) {
    m1.lock();
    m2.lock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t *)arg)
              << "] acquired lock m1/m2 successfully, about to do work\n";
    assert_interrupts_enabled();

    for (volatile size_t i = 0; i < WORK; [](auto &v) {
             auto t = v;
             v = t + 1;
         }(i)) {
        if (!(i % (WORK / 2))) {
            std::cout << "[Child " << *((size_t *)arg) << "] Working\n";
            assert_interrupts_enabled();
        }
    }

    std::cout << "[Child " << *((size_t *)arg) << "] completed work\n";
    m1.unlock();
    m2.unlock();
    std::cout << "[Child " << *((size_t *)arg) << "] unlocked m1/m2\n";
    assert_interrupts_enabled();
    delete (size_t *)arg;
}

void child2(void *arg) {
    m2.lock();
    m3.lock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t *)arg)
              << "] acquired lock m2/m3 successfully, about to do work\n";
    assert_interrupts_enabled();

    for (volatile size_t i = 0; i < WORK; [](auto &v) {
             auto t = v;
             v = t + 1;
         }(i)) {
        if (!(i % (WORK / 2))) {
            std::cout << "[Child " << *((size_t *)arg) << "] Working\n";
            assert_interrupts_enabled();
        }
    }

    std::cout << "[Child " << *((size_t *)arg) << "] completed work\n";
    m2.unlock();
    m3.unlock();
    std::cout << "[Child " << *((size_t *)arg) << "] unlocked m2/m3\n";
    assert_interrupts_enabled();
    delete (size_t *)arg;
}

void child3(void *arg) {
    m3.lock();
    m4.lock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t *)arg)
              << "] acquired lock m3/m4 successfully, about to do work\n";
    assert_interrupts_enabled();

    for (volatile size_t i = 0; i < WORK; [](auto &v) {
             auto t = v;
             v = t + 1;
         }(i)) {
        if (!(i % (WORK / 2))) {
            std::cout << "[Child " << *((size_t *)arg) << "] Working\n";
            assert_interrupts_enabled();
        }
    }

    std::cout << "[Child " << *((size_t *)arg) << "] completed work\n";
    m3.unlock();
    m4.unlock();
    std::cout << "[Child " << *((size_t *)arg) << "] unlocked m3/m4\n";
    assert_interrupts_enabled();
    delete (size_t *)arg;
}

void child4(void *arg) {
    m4.lock();
    m0.lock();
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t *)arg)
              << "] acquired lock m4/m0 successfully, about to do work\n";
    assert_interrupts_enabled();

    for (volatile size_t i = 0; i < WORK; [](auto &v) {
             auto t = v;
             v = t + 1;
         }(i)) {
        if (!(i % (WORK / 2))) {
            std::cout << "[Child " << *((size_t *)arg) << "] Working\n";
            assert_interrupts_enabled();
        }
    }

    std::cout << "[Child " << *((size_t *)arg) << "] completed work\n";
    m4.unlock();
    m0.unlock();
    std::cout << "[Child " << *((size_t *)arg) << "] unlocked m4/m0\n";
    assert_interrupts_enabled();
    delete (size_t *)arg;
}

void parent(void *arg) {
    assert_interrupts_enabled();
    m0.lock();
    m1.lock();
    m2.lock();
    m3.lock();
    m4.lock();

    assert_interrupts_enabled();
    std::cout << "Parent locked m0 - m4 and spawning children\n";

    thread t0(child0, (void *)new size_t(0));
    thread t1(child1, (void *)new size_t(1));
    thread t2(child2, (void *)new size_t(2));
    thread t3(child3, (void *)new size_t(3));
    thread t4(child4, (void *)new size_t(4));

    assert_interrupts_enabled();

    m0.unlock();
    m2.unlock();
    m4.unlock();
    m3.unlock();
    m1.unlock();

    thread::yield();

    assert_interrupts_enabled();

    std::cout << "Parent returning\n";
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), false,
              false, 1);
    return 0;
}