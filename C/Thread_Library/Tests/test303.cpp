#include <unistd.h>

#include <deque>
#include <iostream>
#include <memory>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

const int NUM_THREADS = 12;

mutex m;
cv c;
bool turn = false;

void child_type1(void* arg) {
    m.lock();
    thread::yield();
    while (!turn) {
        c.wait(m);
    }
    thread::yield();
    int a = 5;
    thread::yield();
    a = a + 2 - 5 + 4 - 2 + 10 * 2;
    thread::yield();
    a *= a + 2 - 5 + 4 - 2 + 10 * 2;
    thread::yield();
    std::cout << "Child type 1 finishing up work!\n";
    thread::yield();

    m.unlock();
}

void child_type2(void* arg) {
    m.lock();
    thread::yield();

    while (!turn) {
        c.wait(m);
    }

    for (int i = 0; i < 5; ++i) {
        thread::yield();
        printf("foo bar!\n");
        thread::yield();
    }
    thread::yield();
    std::cout << "Child type 2 finishing up work!\n";
    thread::yield();
    m.unlock();
}

void child_type3(void* arg) {
    m.lock();
    thread::yield();

    while (!turn) {
        c.wait(m);
    }

    for (int i = 0; i < 3; ++i) {
        thread::yield();
        int j = 5;
        thread::yield();
        j = j + j;
        printf("j is equal to: %d\n", j);
        thread::yield();
        j = j + j;
        thread::yield();
        j = j + j;
        printf("j is equal to: %d\n", j);
        thread::yield();
        j = j + j;
        thread::yield();
        j = j + j;
        thread::yield();
        j = j + j;
        thread::yield();
        for (int k = 0; k < 3; ++k) {
            thread::yield();
            printf("child type 3 cooking!\n");
            thread::yield();
            printf("j is equal to: %d\n", j);
            thread::yield();
        }
    }

    thread::yield();
    std::cout << "Child type 3 finishing up work!\n";
    thread::yield();

    m.unlock();
}

void parent(void* arg) {
    for (int i = 0; i < NUM_THREADS / 3; ++i) {
        printf("yielding inside parent!\n");
        thread::yield();
        thread t(child_type3, (void*)0);
        printf("yielding inside parent!\n");
        thread::yield();
    }
    for (int i = 0; i < NUM_THREADS / 3; ++i) {
        printf("yielding inside parent!\n");
        thread::yield();
        thread t(child_type1, (void*)0);
        printf("yielding inside parent!\n");
        thread::yield();
    }
    for (int i = 0; i < NUM_THREADS / 3; ++i) {
        printf("yielding inside parent!\n");
        thread::yield();
        thread t(child_type2, (void*)0);
        printf("yielding inside parent!\n");
        thread::yield();
    }
    thread::yield();
    turn = true;
    thread::yield();
    c.broadcast();
    thread::yield();
}

int main() {
    cpu::boot(1, parent, static_cast<void*>(const_cast<char*>("[Boot Thread]")), false,
              false, 13124);
    return 0;
}
