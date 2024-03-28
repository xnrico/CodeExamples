#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

// constexpr size_t NUM_THREADS = 5;

mutex m;
cv c;

int count = 0;

bool t1_done = false;
bool t2_done = false;
bool t3_done = false;

void child(void *arg) {
    assert_interrupts_enabled();

    m.lock();

    assert_interrupts_enabled();

    for (int i = 0; i < 1000; ++i) {
        assert_interrupts_enabled();
        ++count;
    }

    assert_interrupts_enabled();

    c.signal();

    assert_interrupts_enabled();

    m.unlock();

    assert_interrupts_enabled();
}

void parent(void *arg) {
    assert_interrupts_enabled();

    m.lock();
    
    assert_interrupts_enabled();

    thread t1(child, (void *)0);

    assert_interrupts_enabled();

    while (count != 1000) {
        assert_interrupts_enabled();
        c.wait(m);
    }

    assert_interrupts_enabled();

    m.unlock();

    assert_interrupts_enabled();
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), true,
              true, 1);
    return 0;
}