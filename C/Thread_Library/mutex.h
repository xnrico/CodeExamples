/*
 * mutex.h -- interface to the mutex class
 *
 * You may add new variables and functions to this class.
 *
 * Do not modify any of the given function declarations.
 */

#pragma once

#include <sys/wait.h>
#include <ucontext.h>

#include <deque>
#include <stdexcept>

#include "cpu.h"
#include "custom_ucontext.h"

#define SIZE_T_MAX_VAL (9223372036854775807L * 2UL + 1UL)

class mutex {
   public:
    mutex();
    ~mutex();

    void lock();
    void lock_helper();
    void unlock();
    void unlock_helper();

    // DEBUGGING PURPOSES:

    /*
     * Disable the copy constructor and copy assignment operator.
     */
    mutex(const mutex&) = delete;
    mutex& operator=(const mutex&) = delete;

    /*
     * Move constructor and move assignment operator.  Implementing these is
     * optional in Project 2.
     */
    mutex(mutex&&);
    mutex& operator=(mutex&&);

    bool free;

    size_t holder_id = SIZE_T_MAX_VAL;
    std::deque<std::unique_ptr<my_ucontext_t>> waitingQueue;
};