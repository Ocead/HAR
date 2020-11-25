//
// Created by Johannes on 22.11.2020.
//

#ifndef HAR_SIMPLE_TIMER_HPP
#define HAR_SIMPLE_TIMER_HPP

#include <chrono>
#include <functional>
#include <mutex>
#include <thread>

#include <har/types.hpp>

namespace har {

    class simple_timer {
    private:
        std::chrono::microseconds _timeout;
        clock::time_point _last_begin;
        std::function<void()> _lambda;
        std::atomic_flag _valid;
        std::atomic_flag _run;
        std::mutex _mutex;
        std::thread _thread;

    public:
        explicit simple_timer(decltype(_lambda) && lambda,
                              decltype(_timeout) timeout = decltype(_timeout)()) :
                _timeout(timeout),
                _last_begin(clock::now()),
                _lambda(lambda),
                _valid(true),
                _run(false),
                _mutex(),
                _thread() {
            _mutex.lock();
            _thread = std::thread([this] {
                do {
                    if (_run.test_and_set(std::memory_order_acquire)) {
                        _last_begin = clock::now();
                        _lambda();
                        //TODO: Maybe account for scheduling with micro timings
                        std::this_thread::sleep_until(_last_begin + _timeout);
                    } else {
                        _run.clear(std::memory_order_release);
                        _mutex.lock();
                    }
                } while (_valid.test_and_set(std::memory_order_acquire));
            });
        }

        [[nodiscard]]
        std::chrono::microseconds timeout() const {
            return _timeout;
        }

        void start() {
            if (!_run.test_and_set(std::memory_order_acq_rel)) {
                _mutex.unlock();
            }
        }

        void start(std::chrono::microseconds timeout) {
            _timeout = timeout;
            start();
        }

        void stop() {
            _run.clear(std::memory_order_release);
        }

        ~simple_timer() noexcept {
            _valid.clear(std::memory_order_release);
            start(std::chrono::microseconds());
            if (_thread.joinable()) {
                _thread.join();
            }
        }
    };

}

#endif //HAR_SIMPLE_TIMER_HPP
