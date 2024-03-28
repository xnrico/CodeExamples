// Many threads sharing mutexes!
#include <deque>
#include <memory>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

const size_t NUM_THREADS = 12;

mutex m1;
mutex m2;
mutex m3;

int count1 = 0;
int count2 = 0;
int count3 = 0;
int broken_count = 0;

struct args {
    mutex& m;
    int group;
};

void child(void* arg) {
    args arguments = *(args*)arg;

    for (size_t i = 0; i < 20; ++i) {
        thread::yield();
        arguments.m.lock();
        thread::yield();

        if (arguments.group == 3) {
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
        thread::yield();
    }
    return;
}

void parent(void* arg) {
    std::deque<thread> d;

    for (size_t i = 0; i < NUM_THREADS; ++i) {
        if (i % 3 == 0) {
            d.emplace_back(child, (void*)new args{m3, 3});
        } else if (i % 2 == 0) {
            d.emplace_back(child, (void*)new args{m2, 2});
        } else {
            d.emplace_back(child, (void*)new args{m1, 1});
        }
    }

    for (size_t i = 0; i < d.size(); ++i) {
        d[i].join();
    }

    printf(
        "All threads completed! Count1 is %d, count2 is %d, count3 is %d, broken_count "
        "is %d\n",
        count1, count2, count3, broken_count);
}

int main() {
    cpu::boot(1, parent, static_cast<void*>(const_cast<char*>("[Boot Thread]")), false,
              false, 13124);
    return 0;
}