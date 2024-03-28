#include <iostream>

#include "cpu.h"
#include "thread.h"

int g = 0;

void loop(void *a) {
    assert_interrupts_enabled();

    auto id = static_cast<char *>(a);

    assert_interrupts_enabled();

    int i;

    assert_interrupts_enabled();

    for (i = 0; i < 3; i++) {
        assert_interrupts_enabled();
        std::cout << id << ":\t" << i << "\t" << g << std::endl;
        ++g;
    }

    assert_interrupts_enabled();
}

void parent(void *a) {
    assert_interrupts_enabled();

    thread t1(loop, static_cast<void *>(const_cast<char *>("[Child Thread 1]")));

    assert_interrupts_enabled();

    loop(static_cast<void *>(const_cast<char *>("[Boot Thread]")));

    assert_interrupts_enabled();
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), true,
              true, 1);
    return 0;
}