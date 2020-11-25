//
// Created by Johannes on 22.11.2020.
//

#ifndef HAR_BARRIER_HPP
#define HAR_BARRIER_HPP

#include <atomic>
#include <cstddef>
#include <mutex>

#include <har/types.hpp>

namespace har {

    class barrier {
    private:
        std::atomic<std::ptrdiff_t> _count;
        const std::ptrdiff_t _expected;

        std::mutex _mutex;

        [[nodiscard]]
        bool_t finished() const;

    public:
        explicit barrier(std::ptrdiff_t expected);

        void arrive(std::ptrdiff_t n = 1);

        void wait();

        void arrive_and_wait(std::ptrdiff_t n);

        void reset();

        ~barrier() noexcept;
    };

}


#endif //HAR_BARRIER_HPP
