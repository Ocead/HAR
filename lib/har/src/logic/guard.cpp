//
// Created by Johannes on 09.08.2020.
//

#include "logic/guard.hpp"

using namespace har;

guard::guard() : _count(), _mutex() {
    _mutex.lock();
}

bool_t guard::lock(bool_t block) {
    uint_t c = _count.fetch_add(1, std::memory_order_acq_rel);
    DEBUG_LOG(std::string("Lock guard from ") + std::to_string(c));
    if (c > 0 && block) {
        DEBUG_LOG("Wait on mutex");
        _mutex.lock();
        DEBUG_LOG("Locked mutex");
    }
    return c == 0;
}

bool_t guard::unlock(bool_t block) {
    uint_t c = _count.fetch_sub(1, std::memory_order_acq_rel);
    DEBUG_LOG(std::string("Unlock guard to ") + std::to_string(c));
    if (c > 1 && block) {
        _mutex.unlock();
        DEBUG_LOG("Unlocked mutex");
    }
    return c == 0;
}

guard::~guard() noexcept {
    _mutex.unlock();
}
