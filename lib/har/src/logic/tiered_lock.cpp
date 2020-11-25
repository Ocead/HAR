//
// Created by Johannes on 22.11.2020.
//

#include "logic/tiered_lock.hpp"

using namespace har;

tiered_lock::tiered_lock() :
        _t0ex(),
        _t1ex(),
        _t0_count(0u),
        _t1_count(0u) {

}

void tiered_lock::lock_high_tier() {
    _t0_count.fetch_add(1u, std::memory_order_acq_rel);
    _t0ex.lock();
}

void tiered_lock::lock_low_tier() {
    _t1_count.fetch_add(1u, std::memory_order_acq_rel);
    _t1ex.lock();
    lock_high_tier();
}

void tiered_lock::block_low_tier() {
    _t1ex.lock();
}

void tiered_lock::unblock_low_tier() {
    _t1ex.unlock();
}

void tiered_lock::unlock_high_tier() {
    _t0ex.unlock();
    _t0_count.fetch_sub(1u, std::memory_order_acq_rel);
}

void tiered_lock::unlock_low_tier() {
    unlock_high_tier();
    _t1_count.fetch_sub(1u, std::memory_order_acq_rel);
    _t1ex.unlock();
}

tiered_lock::~tiered_lock() noexcept {
    if (_t0_count.load(std::memory_order_acquire) > 0) {
        _t0ex.unlock();
    }

    if (_t1_count.load(std::memory_order_acquire) > 0) {
        _t1ex.unlock();
    }
}
