//
// Created by Johannes on 22.11.2020.
//

#include "logic/barrier.hpp"

using namespace har;

barrier::barrier(std::ptrdiff_t expected) :
        _count(expected),
        _expected(expected),
        _mutex() {

}

bool_t barrier::finished() const {
    return _count.load(std::memory_order_acquire) == 0;
}

void barrier::arrive(std::ptrdiff_t n) {
    if (_count.fetch_sub(n, std::memory_order_acq_rel) <= 1) {
        _mutex.unlock();
    }
}

void barrier::wait() {
    std::scoped_lock{ _mutex };
}

void barrier::arrive_and_wait(std::ptrdiff_t n) {
    if (_count.fetch_sub(n, std::memory_order_acq_rel) <= 1) {
        _mutex.unlock();
    } else {
        wait();
    }
}

void barrier::reset() {
    _count.exchange(_expected, std::memory_order_acq_rel);
    _mutex.lock();
}

barrier::~barrier() noexcept {
    wait();
}
