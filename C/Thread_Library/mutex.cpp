#include "mutex.h"

mutex::mutex() {
    // Disable interrupts to ensure atomicity
    cpu::interrupt_disable();
    free = true; // Initialize mutex as free
    cpu::interrupt_enable(); // Re-enable interrupts
}

mutex::~mutex() {}

void mutex::lock() {
    cpu::interrupt_disable(); // Disable interrupts for critical section
    lock_helper(); // Call helper function to perform locking operations
    cpu::interrupt_enable(); // Re-enable interrupts after critical section
}

void mutex::lock_helper() {
    if (!free) { // If mutex is not free
        // Add current thread to the waiting queue
        waitingQueue.push_back(std::move(thread::currentContext));

        if (!thread::readyQueue.empty()) {
            // Move the next ready thread to current context and swap contexts
            thread::currentContext = std::move(std::move(thread::readyQueue.front()));
            thread::readyQueue.pop_front();

            swapcontext(waitingQueue.back()->ucp.get(),
                        thread::currentContext->ucp.get());
        } else {
            // No runnable threads, suspend interrupts
            cpu::interrupt_enable_suspend();
        }
    } else {
        // Lock the mutex and set current thread as holder
        free = false;
        this->holder_id = thread::currentContext->id;
    }
}

void mutex::unlock() {
    cpu::interrupt_disable(); // Disable interrupts for critical section
    unlock_helper(); // Call helper function to perform unlocking operations
    cpu::interrupt_enable(); // Re-enable interrupts after critical section
}

void mutex::unlock_helper() {
    // Check if the current thread owns the mutex
    if (this->holder_id != thread::currentContext->id) {
        cpu::interrupt_enable();
        throw std::runtime_error("\nmutex::unlock(): Unlocking an unowned Mutex");
    }

    if (!waitingQueue.empty()) {
        // Transfer ownership to the next waiting thread
        this->holder_id = this->waitingQueue.front()->id;

        // Move the waiting thread to ready queue
        thread::readyQueue.push_back(std::move(waitingQueue.front()));
        waitingQueue.pop_front();
    } else {
        // No waiting threads, release mutex
        this->holder_id = SIZE_T_MAX_VAL;
        free = true;
    }
}

mutex::mutex(mutex &&other) {
    // Move constructor should not be used
    throw std::runtime_error("\nmutex::mutex(&&): Invalid Use of Mutex Move Constructor");
}

mutex &mutex::operator=(mutex &&other) {
    // Move assignment operator should not be used
    throw std::runtime_error("\nmutex::mutex(&&):: Invalid Use of Mutex Move Copy Constructor");
}
