#include <deque>
#include <iostream>

#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include "thread.h"

// for the code to work correctly, this must be an ODD NUMBER! BE CAREFUL!
constexpr long int NUM_CHILDREN = 10;

mutex m;
cv parent_cv;
cv child_cv;

int global_id = 0;
int global_track_id = 7;

int active_threads = 0;

int next_thread_id = 0;

struct child_args {
    int id;
    int track_id;
};

void child(void *arg) {
    assert_interrupts_enabled();

    m.lock();

    assert_interrupts_enabled();

    child_args c = *(child_args *)arg;

    assert_interrupts_enabled();

    while (c.id != next_thread_id) {
        child_cv.wait(m);
        assert_interrupts_enabled();
    }

    assert_interrupts_enabled();

    printf("child id %d is now running track_id %d\n", c.id, c.track_id);

    assert_interrupts_enabled();

    next_thread_id = (next_thread_id + 2) % NUM_CHILDREN;

    assert_interrupts_enabled();

    --active_threads;

    assert_interrupts_enabled();

    printf("child id %d is now waking up [parent]\n", c.id);

    assert_interrupts_enabled();

    parent_cv.signal();

    assert_interrupts_enabled();

    m.unlock();

    assert_interrupts_enabled();

    printf("child id %d unlocks\n", c.id);

    assert_interrupts_enabled();
}

void parent(void *arg) {
    assert_interrupts_enabled();

    m.lock();

    assert_interrupts_enabled();

    std::deque<thread> threads;

    assert_interrupts_enabled();

    printf("Parent started! Spawning %ld children!\n", NUM_CHILDREN);

    assert_interrupts_enabled();

    for (long int i = 0; i < NUM_CHILDREN; ++i) {
        assert_interrupts_enabled();

        threads.emplace_back(
            child, (void *)new child_args{global_id++, (global_track_id * 8 + 77) % 149});

        ++active_threads;

        global_track_id = global_track_id * 8 + 77;

        assert_interrupts_enabled();
    }

    assert_interrupts_enabled();

    while (active_threads != 0) {
        assert_interrupts_enabled();

        printf("parent goes to sleep\n");

        assert_interrupts_enabled();

        parent_cv.wait(m);

        printf("parent wakes up\n");

        assert_interrupts_enabled();

        child_cv.broadcast();

        assert_interrupts_enabled();
    }

    assert_interrupts_enabled();

    printf("Parent ended!\n");

    assert_interrupts_enabled();
}

int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), false,
              false, 1);
    return 0;
}