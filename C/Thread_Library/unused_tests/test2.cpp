#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

int g = 0;
mutex mtx1;
cv cv1;

void loop(void *a) {
    assert_interrupts_enabled();
    auto id = static_cast<char *>(a);
    int i;

    assert_interrupts_enabled();

    mtx1.lock();

    assert_interrupts_enabled();

    for (i = 0; i < 5000; i++) {
        std::cout << id << ":\t" << i << "\t" << g << std::endl;
        ++g;
    }

    assert_interrupts_enabled();

    mtx1.unlock();

    assert_interrupts_enabled();
}

void parent(void *a) {
    assert_interrupts_enabled();

    auto arg = reinterpret_cast<char *>(a);

    assert_interrupts_enabled();

    std::cout << "void parent() called with arg " << arg << " upon CPU boot...\n";

    assert_interrupts_enabled();

    thread t1(loop, static_cast<void *>(const_cast<char *>("[Child Thread 1]")));

    assert_interrupts_enabled();

    thread t2(loop, static_cast<void *>(const_cast<char *>("[Child Thread 2]")));

    assert_interrupts_enabled();

    thread t3(loop, static_cast<void *>(const_cast<char *>("[Child Thread 3]")));

    assert_interrupts_enabled();

    thread t4(loop, static_cast<void *>(const_cast<char *>("[Child Thread 4]")));

    assert_interrupts_enabled();
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), true,
              true, 1);
    return 0;
}