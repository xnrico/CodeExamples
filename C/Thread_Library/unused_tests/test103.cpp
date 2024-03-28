#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

#define WORK 10000000

// MUTEX TEST 103, Dead lock (Forced)

mutex m1;
mutex m2;

void child0(void *arg) {
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] is acquiring lock m1\n";
    m1.lock();

    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] acquired m1 successfully\n";

    thread::yield();

    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] is acquiring lock m2\n";
    m2.lock();

    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] acquired m2 successfully, doing work\n";

    
    for (volatile size_t i = 0; i < WORK; [](auto& v) { auto t = v; v = t + 1; } (i)) {
        if (!(i%(WORK/2))) {
            std::cout << "[Child " << *((size_t*) arg) << "] Bleeehhh\n";
            assert_interrupts_enabled();
        }
    }

    std::cout << "[Child " << *((size_t*) arg) << "] completed work, unlocking m2\n";
    assert_interrupts_enabled();
    m2.unlock();

    std::cout << "[Child " << *((size_t*) arg) << "] then unlocking m1\n";
    assert_interrupts_enabled();
    m1.unlock();

    assert_interrupts_enabled();

    std::cout << "[Child " << *((size_t*) arg) << "] unlocked both sucessfully, deleting then returning\n";
    delete (size_t*)arg;
}

void child1(void *arg) {
    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] is acquiring lock m2\n";
    m2.lock();

    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] acquired m2 successfully\n";

    thread::yield();

    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] is acquiring lock m1\n";
    m1.lock();

    assert_interrupts_enabled();
    std::cout << "[Child " << *((size_t*) arg) << "] acquired m1 successfully, doing work\n";

    
    for (volatile size_t i = 0; i < WORK; [](auto& v) { auto t = v; v = t + 1; } (i)) {
        if (!(i%(WORK/2))) {
            std::cout << "[Child " << *((size_t*) arg) << "] Bleeehhh\n";
            assert_interrupts_enabled();
        }
    }

    std::cout << "[Child " << *((size_t*) arg) << "] completed work, unlocking m1\n";
    assert_interrupts_enabled();
    m1.unlock();

    std::cout << "[Child " << *((size_t*) arg) << "] then unlocking m2\n";
    assert_interrupts_enabled();
    m2.unlock();

    assert_interrupts_enabled();
    
    std::cout << "[Child " << *((size_t*) arg) << "] unlocked both sucessfully, deleting then returning\n";
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