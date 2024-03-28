/* Basic test case to make sure signal does NOT have a memory! */

#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

mutex m;
cv c1;
cv c2;

void child(void *arg) {
    m.lock();
    c2.signal();
    c1.wait(m);
    m.unlock();
}

void parent(void *arg) {
    m.lock();
    c1.signal();
    thread t(child, 0);
    c2.wait(m);
    c1.signal();
    m.unlock();
}

int main() {
    cpu::boot(1, parent, 0, true, true, 1);
    return 0;
}
