#include <iostream>
#include <vector>
#include "cpu.h"
#include "thread.h"
#include "mutex.h"
#include "cv.h"

#define NUM_THREADS 1 // 1 Million Threads
constexpr size_t GB = 1024 * 1024 * 1024; // 1GB in bytes
constexpr size_t TOTAL_GB = 65535; // Total GB to allocate
constexpr size_t TOTAL_BYTES = TOTAL_GB * GB; // Total bytes to allocat

// Test 10 Try to blow up Memory inside User Code and see how std::bad_alloc() can be thrown...

void child(void *a)
{
    std::vector<char*> allocated_blocks;

    // Allocate memory in 1GB increments until 128GB is reached
    size_t bytes_allocated = 0;
    while (bytes_allocated < TOTAL_BYTES) {
        // Allocate 1GB
        try {
            char* ptr = new char[GB];
            allocated_blocks.push_back(ptr);
            bytes_allocated += GB;
            if (bytes_allocated % (16*GB) == 0) {
                std::cout << "Allocated 1GB. Total allocated: " << bytes_allocated / GB << "GB\n";
            }
        } catch (const std::bad_alloc& e) {
            std::cerr << "Memory allocation failed: " << e.what() << std::endl;
            break; // Stop allocation if unable to allocate more memory
        }
    }

    // Deallocate memory
    while (!allocated_blocks.empty()) {
        delete[] allocated_blocks.back(); // Delete the last allocated block
        allocated_blocks.pop_back(); // Remove the last element from the vector
        bytes_allocated -= GB; // Adjust the total bytes allocated
        if (bytes_allocated % (16*GB) == 0) {
            std::cout << "Deallocated 1GB. Total allocated: " << bytes_allocated / GB << "GB\n";
        } 
    }

    std::cout << "All memory deallocated.\n";
}

void parent(void *a)
{
    std::cout << "Parent spawning child\n";

    for (size_t i=0; i<NUM_THREADS; ++i) {
        thread t(child, (void*)new int(i));
    }

    std::cout << "Parent finished, returning\n";
}

int main()
{
    cpu::boot(1, parent, reinterpret_cast<void *>(100), false, false, 0);
}