#include <iostream>
#include <vector>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"

#define GB 128
#define NUM_THREADS_PER_GB 4096 // Will take 16GB of Memory if all allocated successfully
#define GIGA_BYTE 1073741824UL

size_t kiloBytes = 0; // Stack Size is 256 KB, Heap size is Idk, 4096 Threads will take 1GB of Memory
size_t num_child = 0;
size_t total_threads = GB * NUM_THREADS_PER_GB;
mutex m;

// Test 9 Try to blow up Memory inside OS Code and see how std::bad_alloc() can be thrown...
// User code does not handle the exception in this test...

void child(void *a)
{
    ++num_child;

    if (*((size_t*) a) % 4096 == 0) {
        std::cout << "[Child " << *((size_t*) a) << "] starts to wait for lock\n";
        std::cout << num_child << " threads are now waiting, using " << (num_child) * 256 / (1024 * 1024) + 1 << " GB of Memory\n";
    }
    
    m.lock();

    m.unlock();
}

void parent(void *a)
{
    m.lock();

    std::cout << "Parent acquired lock, spawning children...\n";

    for (size_t i=0; i<total_threads; ++i) {
        thread t(child, (void*)new int(i));
    }

    m.unlock();

    std::cout << "Parent finished spawning and unlocked, exiting...\n";
}

int main()
{
    cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}