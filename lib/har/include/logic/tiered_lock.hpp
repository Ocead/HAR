//
// Created by Johannes on 22.11.2020.
//

#ifndef HAR_TIERED_LOCK_HPP
#define HAR_TIERED_LOCK_HPP

#include <atomic>
#include <mutex>

#include <har/types.hpp>

namespace har {

    class tiered_lock {
    private:
        std::mutex _t0ex;
        std::mutex _t1ex;
        std::atomic<uint_t> _t0_count;
        std::atomic<uint_t> _t1_count;

    public:
        tiered_lock();

        void lock_high_tier();

        void lock_low_tier();

        void block_low_tier();

        void unblock_low_tier();

        void unlock_high_tier();

        void unlock_low_tier();

        ~tiered_lock() noexcept;
    };

}

#endif //HAR_TIERED_LOCK_HPP
