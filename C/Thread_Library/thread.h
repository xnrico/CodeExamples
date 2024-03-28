/*
 * thread.h -- interface to the thread library
 *
 * This file should be included by the thread library and by application
 * programs that use the thread library.
 *
 * You may add new variables and functions to this class.
 *
 * Do not modify any of the given function declarations.
 */

#pragma once

#if !defined(__cplusplus) || __cplusplus < 201700L
#error Please configure your compiler to use C++17 or C++20
#endif

#include <ucontext.h>

#include <deque>
#include <map>
#include <memory>

#include "cpu.h"
#include "custom_ucontext.h"

class mutex;
class cv;

static constexpr unsigned int STACK_SIZE =
    262144;  // size of each thread's stack in bytes

using thread_startfunc_t = void (*)(void*);

class thread {
   public:
    thread(thread_startfunc_t func, void* arg);            // Constructor for User
    thread(thread_startfunc_t func, void* arg, bool os);   // Constructor for Library
    void ctor_helper(thread_startfunc_t func, void* arg);  // Helper for Constructors

    ~thread();

    void join();

    static void yield();

    /*
     * Disable the copy constructor and copy assignment operator.
     */
    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;

    /*
     * Move constructor and move assignment operator.  Implementing these is
     * optional in Project 2.
     */
    thread(thread&&);
    thread& operator=(thread&&);

    // Textbook 4.6
    static void thread_start(thread*, thread_startfunc_t, void*);
    static void thread_exit(int);
    // Interrupt Handler
    static void timer_interrupt();

    // Static variables
    static std::unique_ptr<my_ucontext_t> currentContext;
    static std::deque<std::unique_ptr<my_ucontext_t>> readyQueue;
    static std::deque<std::unique_ptr<my_ucontext_t>> finishedQueue;

    static size_t thread_id;  // Static threadID to assign to new threads
    size_t id;                // This thread's ID

    // Needed for join implementation
    static std::map<size_t, std::deque<std::unique_ptr<my_ucontext_t>>> waitingThreads;
    static std::deque<bool> threadHasCompleted;
};