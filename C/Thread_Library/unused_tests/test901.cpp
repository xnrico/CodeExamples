#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

constexpr long int NUM_CHILDREN = 102;

mutex m;
cv c;

size_t count = 0;

void child(void *arg) {
    assert_interrupts_enabled();

    size_t id = (size_t)(*((int *)arg));

    assert_interrupts_enabled();

    for (int i = 0; i < 10; ++i) {
        assert_interrupts_enabled();

        m.lock();

        assert_interrupts_enabled();

        // std::cout << "This is thread " << id << "! Count is: " << count
        //           << "! incrementing!\n";

        assert_interrupts_enabled();

        ++count;

        assert_interrupts_enabled();

        m.unlock();

        assert_interrupts_enabled();
    }

    assert_interrupts_enabled();

    std::cout << "Child " << id << " ended!\n";

    assert_interrupts_enabled();
}

void parent(void *arg) {
    assert_interrupts_enabled();

    m.lock();

    assert_interrupts_enabled();

    std::deque<thread> threads;

    assert_interrupts_enabled();

    std::cout << "Parent started! Spawning " << NUM_CHILDREN << " children!\n";

    assert_interrupts_enabled();

    for (size_t i = 0; i < NUM_CHILDREN; ++i) {
        assert_interrupts_enabled();
        threads.emplace_back(child, (void *)new int(i));
    }

    assert_interrupts_enabled();

    m.unlock();

    assert_interrupts_enabled();

    for (size_t i = 0; i < NUM_CHILDREN; ++i) {
        assert_interrupts_enabled();
        threads[i].join();
        assert_interrupts_enabled();
    }

    std::cout << "Parent Finished too... Count is " << count << "\n";
    assert_interrupts_enabled();
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), false,
              false, 1);
    return 0;
}
