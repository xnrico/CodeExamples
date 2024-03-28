#pragma once

#include <ucontext.h>

#include <memory>

typedef struct my_ucontext_t {
    void* sp;
    size_t id;
    std::unique_ptr<ucontext_t> ucp;
} my_ucontext_t;
