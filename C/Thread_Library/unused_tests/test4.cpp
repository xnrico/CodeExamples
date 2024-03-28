// TODO! Write a test for broadcast!
// TODO! Fix join

#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

mutex m;
cv c;

size_t count = 0;

void child(void *arg) {
    assert_interrupts_enabled();

    for (int i = 0; i < 100; ++i) {
        assert_interrupts_enabled();
        ++count;
    }

    assert_interrupts_enabled();
}

void parent(void *arg) {
    assert_interrupts_enabled();
    
    m.lock();

    assert_interrupts_enabled();

    thread t(child, (void *)new int(1));

    assert_interrupts_enabled();

    m.unlock();

    assert_interrupts_enabled();

    t.join();

    assert_interrupts_enabled();
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), true,
              true, 1);
    return 0;
}