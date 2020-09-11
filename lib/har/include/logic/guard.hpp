//
// Created by Johannes on 09.08.2020.
//

#ifndef HAR_GUARD_HPP
#define HAR_GUARD_HPP

#include <atomic>
#include <mutex>

#include <har/types.hpp>

namespace har {
    class guard {
    private:
        std::atomic<uint_t> _count;
        std::mutex _mutex;

    public:
        guard();

        bool_t lock(bool_t block);

        bool_t unlock(bool_t block);

        ~guard() noexcept;
    };
}

#endif //HAR_GUARD_HPP
