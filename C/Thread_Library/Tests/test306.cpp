// Many threads sharing mutexes!
#include <deque>
#include <memory>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

const size_t NUM_THREADS = 25;

mutex m1;
mutex m2;
mutex m3;
mutex m4;
mutex m5;

int count1 = 0;
int count2 = 0;
int count3 = 0;
int count4 = 0;
int count5 = 0;
int broken_count = 0;

struct args {
    mutex& m;
    int group;
};

void child(void* arg) {
    args arguments = *(args*)arg;

    for (int i = 0; i < 20; ++i) {
        thread::yield();
        arguments.m.lock();
        thread::yield();
        arguments.m.unlock();
        thread::yield();
        arguments.m.lock();
        thread::yield();
        thread::yield();
        arguments.m.unlock();
        thread::yield();
        thread::yield();
        arguments.m.lock();
        thread::yield();

        if (arguments.group == 5) {
            count5++;
        } else if (arguments.group == 4) {
            count4++;
        } else if (arguments.group == 3) {
            count3++;
        } else if (arguments.group == 2) {
            count2++;
        } else {  // group is 1
            count1++;
        }

        thread::yield();
        int new_val = broken_count + 5;
        thread::yield();
        broken_count = new_val;
        thread::yield();
        new_val = broken_count - 115;
        thread::yield();
        broken_count = new_val;
        thread::yield();
        new_val = broken_count + 13;
        thread::yield();
        new_val = new_val + 15;
        thread::yield();
        broken_count = new_val + 15 + 113 - 10 + 23 * 2 / 10 + 23 + 10 - 100 + 50 - 23 +
                       100 + 23 - 40 + 14 + 2333 - 40 + 5 / 5 + 23 + 1;
        thread::yield();
        arguments.m.unlock();
        arguments.m.lock();
        arguments.m.unlock();

        thread::yield();
    }
    return;
}

void parent(void* arg) {
    std::deque<thread> d;

    for (int i = 0; i < NUM_THREADS; ++i) {
        if (i % 5 == 0) {
            d.emplace_back(child, (void*)new args{m5, 5});
        } else if (i % 4 == 0) {
            d.emplace_back(child, (void*)new args{m4, 4});
            thread::yield();
        } else if (i % 3 == 0) {
            d.emplace_back(child, (void*)new args{m3, 3});
        } else if (i % 2 == 0) {
            d.emplace_back(child, (void*)new args{m2, 2});
        } else {  // group is 1
            d.emplace_back(child, (void*)new args{m1, 1});
        }
    }

    for (int i = d.size() - 1; i >= 0; --i) {
        d[i].join();
    }

    printf(
        "All threads completed! Count1 is %d, count2 is %d, count3 is %d, count4 is %d, "
        "count5 is %d, broken_count "
        "is %d\n",
        count1, count2, count3, count4, count5, broken_count);

    thread::yield();
}

int main() {
    cpu::boot(1, parent, static_cast<void*>(const_cast<char*>("[Boot Thread]")), false,
              false, 1003);
    return 0;
}