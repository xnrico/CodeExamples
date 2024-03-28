#include "cpu.h"

#include "mutex.h"

cpu::cpu(thread_startfunc_t func, void *arg) {
    interrupt_vector_table[TIMER] = thread::yield;

    if (func != nullptr) {
        // Separate thread constructor for library code.
        thread initial(func, arg, true);
        thread::currentContext = std::move(thread::readyQueue.front());
        thread::readyQueue.pop_front();

        setcontext(thread::currentContext->ucp.get());
    }
}