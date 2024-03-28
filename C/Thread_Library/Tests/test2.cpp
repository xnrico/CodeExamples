#include "thread.h"
#include "cpu.h"
#include "cv.h"
#include "mutex.h"
#include <cassert>
#include <deque>
#include <iostream>

cv cv1;
void test_func(void *a){
    auto mtx = static_cast<mutex* >(a);
    mtx->lock();
    // Simulate long operation
    //cpu::interrupt_enable_suspend();
    cv1.signal();
    mtx->unlock();
    std::cout << "child1 passed!\n";

}
void test_func2(void *a){
    mutex* mtx= static_cast<mutex* >(a);
    mtx->lock();
    std::cout << "child2 locked!\n";
    cv1.wait(*(mtx));
    mtx->unlock();
    std::cout << "child2 finished!\n";
}
void test_priority_inversion() {
    std::cout << "test1 started!\n";
    mutex mtx;
    auto arg = reinterpret_cast<void* >(&mtx);
    thread t1(test_func2, arg );
    thread t2(test_func, arg);
    std::cout << "threads started!\n";
    t1.join();
    t2.join();
    std::cout << "test1 passed!\n";

}


struct tem{
    int* count;
    mutex* mtx;
};

void func3(void *a){
    auto mtx= static_cast<tem* >(a);
    mtx->mtx->lock();
    *(mtx->count)=*(mtx->count)+1;
    cv1.wait(*(mtx->mtx));
    *(mtx->count)=*(mtx->count)+1;
    mtx->mtx->unlock();

}
void func4(void *a){
    auto mtx= static_cast<tem* >(a);
    mtx->mtx->lock();
    *(mtx->count)=*(mtx->count)+1;
    cv1.signal();
    cv1.signal();
    mtx->mtx->unlock();

}
void test_condition_variable_signal_ordering() {
    mutex mtx;
    //std::atomic<int> count(0);
    int count = 0;
    tem arg = {&count, &mtx};
    auto arg2 = reinterpret_cast<void* >(&arg);

    thread t1(func3, arg2);

    thread t2(func3, arg2);

    thread t3(func4, arg2);

    t1.join();
    t2.join();
    t3.join();

    // Ensure the signaling order is fair
    assert(count==5);
}
struct tem3{
    std::deque<int>* order;
    mutex* mtx;
};
void func5(void *a){
    auto mtx= static_cast<tem3* >(a);
    mtx->mtx->lock();
    cv1.wait(*(mtx->mtx));
    mtx->order->push_back(1);
    mtx->mtx->unlock();
}
void func6(void *a){
    auto mtx= static_cast<tem3* >(a);
    mtx->mtx->lock();
    cv1.wait(*(mtx->mtx));
    mtx->order->push_back(2);
    mtx->mtx->unlock();
}
void func7(void *a){
    auto mtx= static_cast<mutex* >(a);
    mtx->lock();
    cv1.signal();
    mtx->unlock();
}
void test_condition_variable_signal_order_bug() {
    std::deque<int> order;
    mutex mtx;
    cv cv;
    tem3 arg = {&order, &mtx};

    thread t1(func5, static_cast<void *>(&arg));

    thread t2(func6, static_cast<void *>(&arg));

    thread t3(func7, static_cast<void *>(&mtx));

    thread t4(func7, static_cast<void *>(&mtx));

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    // Ensure the signaling order is fair
    assert(order.size() == 2);
    assert(order[0] == 1);
    assert(order[1] == 2);
}

void parent(void *arg){
    test_priority_inversion();

    test_condition_variable_signal_ordering();
    test_condition_variable_signal_order_bug();

    std::cout << "All tests passed!\n";

}
int main() {
    cpu::boot(1, parent, static_cast<void *>(const_cast<char *>("[Boot Thread]")), true,
              true, 1);
    return 0;
}
