#include <unistd.h>

#include <deque>
#include <iostream>
#include <memory>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

const int NUM_CHILDREN_PER_PARENT = 10;
int num = 5;

mutex m1;
mutex m2;
cv c1;
cv c2;
bool turn1 = false;
bool turn2 = false;

int group1_num = 0;
int group2_num = 0;

void child_type1(void* arg) {
    m1.lock();
    group1_num = (group1_num + 5) / 2;
    thread::yield();
    while (!turn1) {
        c1.wait(m1);
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
    printf("val of group1 num: %d\n", group1_num);
    thread::yield();
    m1.unlock();
}

void child_type2(void* arg) {
    m1.lock();

    thread::yield();
    thread::yield();
    group1_num = (group1_num + 5) / 2;

    while (!turn1) {
        c1.wait(m1);
    }

    for (int i = 0; i < 5; ++i) {
        thread::yield();
        printf("foo bar!\n");
        thread::yield();
    }
    thread::yield();
    std::cout << "Child type 2 finishing up work!\n";
    thread::yield();
    printf("val of group1 num: %d\n", group1_num);
    m1.unlock();
}

void child_type3(void* arg) {
    m2.lock();
    group2_num = ((group1_num + 146) * 2) / 13;
    thread::yield();

    while (!turn2) {
        c2.wait(m2);
    }

    for (int i = 0; i < 3; ++i) {
        thread::yield();
        int j = 5;
        thread::yield();
        j = j + j;
        thread::yield();
        j = j + j;
        thread::yield();
        j = j + j;
        thread::yield();
        j = j + j;
        thread::yield();
        thread::yield();
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
    num = num + 146;
    thread::yield();
    printf("num is %d\n", num);
    printf("val of group2 num: %d\n", group1_num);
    thread::yield();
    thread::yield();
    std::cout << "Child type 3 finishing up work!\n";
    thread::yield();

    m2.unlock();
}

void child_type4(void* arg) {
    m2.lock();
    thread::yield();
    group2_num = ((group1_num + 13222) * 2) / 20;

    while (!turn2) {
        c2.wait(m2);
    }

    for (int i = 0; i < 10; ++i) {
        thread::yield();
        num += 15;
        thread::yield();
        printf("num is %d\n", num);
        thread::yield();
    }

    thread::yield();
    printf("Lazy child4 not doing any work!\n");
    thread::yield();
    std::cout << "Child type 4 finishing up work!\n";
    thread::yield();

    m2.unlock();
}

void parent1(void* arg) {
    std::deque<thread> d;
    thread::yield();
    printf("Parent 1 starting duty!\n");
    thread::yield();
    for (int i = 0; i < NUM_CHILDREN_PER_PARENT / 2; ++i) {
        thread::yield();
        d.emplace_back(child_type1, (void*)0);
    }
    thread::yield();
    for (int i = 0; i < NUM_CHILDREN_PER_PARENT / 2; ++i) {
        thread::yield();
        d.emplace_back(child_type1, (void*)0);
        thread::yield();
    }
    thread::yield();
    printf("val of group1 num: %d\n", group1_num);
    thread::yield();

    turn1 = true;
    thread::yield();
    c1.broadcast();
    thread::yield();
    d[0].join();
    printf("val of group1 num: %d\n", group1_num);
    d[1].join();
    thread::yield();
    d[2].join();
    d[3].join();
    d[4].join();
    printf("Parent 1 completed spawning all threads. Exiting.\n");
    thread::yield();
    d[5].join();
    thread::yield();
    thread::yield();
    d[6].join();
    thread::yield();
    thread::yield();
    thread::yield();
    d[7].join();
    thread::yield();
    d[8].join();
    thread::yield();
    d[9].join();

    printf("val of group1 num: %d\n", group1_num);
}

void parent2(void* arg) {
    std::deque<thread> d;
    thread::yield();
    printf("Parent 2 starting duty!\n");
    thread::yield();
    for (int i = 0; i < NUM_CHILDREN_PER_PARENT / 2; ++i) {
        thread::yield();
        d.emplace_back(child_type3, (void*)0);
    }
    thread::yield();
    for (int i = 0; i < NUM_CHILDREN_PER_PARENT / 2; ++i) {
        thread::yield();
        d.emplace_back(child_type4, (void*)0);
        thread::yield();
    }
    thread::yield();

    turn2 = true;
    thread::yield();
    c2.broadcast();
    printf("val of group2 num: %d\n", group1_num);
    d[0].join();
    thread::yield();
    d[1].join();
    thread::yield();
    thread::yield();
    printf("val of group2 num: %d\n", group1_num);
    thread::yield();
    d[2].join();
    d[3].join();
    thread::yield();
    d[4].join();
    thread::yield();
    d[9].join();
    thread::yield();
    d[8].join();
    thread::yield();
    d[5].join();
    d[6].join();
    d[7].join();
    printf("Parent 2 completed spawning all threads. Exiting.\n");
    thread::yield();
    printf("val of group2 num: %d\n", group1_num);
    thread::yield();
}

void grand_parent(void* arg) {
    thread p1(parent1, (void*)0);
    thread p2(parent2, (void*)0);

    thread::yield();
    p1.join();
    p1.join();
    thread::yield();
    p2.join();
    printf("Grandparent thread exiting...\n");
    thread::yield();
    p2.join();
    p2.join();
    p2.join();
    thread::yield();
    thread::yield();
    thread::yield();
}

int main() {
    cpu::boot(1, grand_parent, static_cast<void*>(const_cast<char*>("[Boot Thread]")),
              true, true, 13124);
    return 0;
}
