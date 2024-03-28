#include "cv.h"

// Constructor
cv::cv() {}

// Destructor
cv::~cv() {}

// Wait function for condition variable
void cv::wait(mutex& mtx) {
    cpu::interrupt_disable();

    // Release the lock while waiting
    mtx.unlock_helper();

    // Main logic of waiting
    if (!thread::readyQueue.empty()) {
        cvQueue.push_back(std::move(thread::currentContext));
        thread::currentContext = std::move(thread::readyQueue.front());
        thread::readyQueue.pop_front();
        thread::currentContext->ucp.get();
        swapcontext(cvQueue.back()->ucp.get(), thread::currentContext->ucp.get());
    } else {
        // If no threads are ready to run, enable interrupts and suspend
        cpu::interrupt_enable_suspend();
        return;
    }

    // Reacquire the lock after waking up
    mtx.lock_helper();

    // Re-enable interrupts
    cpu::interrupt_enable();
}

// Signal function for condition variable
void cv::signal() {
    cpu::interrupt_disable();

    // Move a thread from the condition variable queue to the ready queue
    if (!cvQueue.empty()) {
        thread::readyQueue.push_back(std::move(cvQueue.front()));
        cvQueue.pop_front();
    }

    // Re-enable interrupts
    cpu::interrupt_enable();
}

// Broadcast function for condition variable
void cv::broadcast() {
    cpu::interrupt_disable();

    // Move all threads from the condition variable queue to the ready queue
    while (!cvQueue.empty()) {
        thread::readyQueue.push_back(std::move(cvQueue.front()));
        cvQueue.pop_front();
    }

    // Re-enable interrupts
    cpu::interrupt_enable();
}

// Move constructor (not implemented)
cv::cv(cv&& other) {
    throw std::runtime_error("\ncv::cv(&&): Invalid Use of CV Move Constructor");
}

// Move assignment operator (not implemented)
cv& cv::operator=(cv&& other) {
    throw std::runtime_error("\ncv::cv(&&): Invalid Use of CV Move Copy Constructor");
}