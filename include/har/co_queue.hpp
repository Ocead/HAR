//
// Created by Johannes on 08.07.2020.
//

#pragma once

#ifndef HAR_CO_QUEUE_HPP
#define HAR_CO_QUEUE_HPP

#include <atomic>
#include <deque>
#include <mutex>

namespace har {

    ///
    /// \tparam T
    template<typename T>
    class co_queue {
    private:
        std::atomic<unsigned> _quewait; ///<
        std::mutex _queex; ///<
        std::deque<T> _queue; ///<

        static_assert(decltype(_quewait)::is_always_lock_free, "Atomic counter is not lock-free!");

        ///
        void lock() {
            if (_quewait++) {
                _queex.lock();
            }
        }

        ///
        void unlock() {
            if (--_quewait) {
                _queex.unlock();
            }
        }

    public:

        /// \brief Constructor
        co_queue() : _quewait(0),
                     _queex(),
                     _queue() {

        }

        ///
        /// \return
        bool empty() {
            lock();
            bool empty = _queue.empty();
            unlock();
            return empty;
        }

        ///
        /// \param ref
        void push(const T & ref) {
            lock();
            _queue.push_front(ref);
            unlock();
        }

        ///
        /// \param fref
        void push(T && fref) {
            lock();
            _queue.push_front(std::forward<T>(fref));
            unlock();
        }

        T pop() {
            lock();
            T t = _queue.back();
            _queue.pop_back();
            unlock();
            return t;
        }

        ///
        /// \return
        std::tuple<T, bool> pop_check() {
            lock();
            T t = _queue.back();
            _queue.pop_back();
            bool empty = _queue.empty();
            unlock();
            return std::make_tuple(t, empty);
        }

        void process_one(std::function<void(T &)> && consumer) {
            lock();
            if (!_queue.empty()) {
                auto p = _queue.back();
                _queue.pop_back();
                unlock();
                consumer(p);
                return;
            }
            unlock();
        }

        void process_all(std::function<void(T &)> && consumer) {
            lock();
            while (!_queue.empty()) {
                auto p = _queue.back();
                _queue.pop_back();
                unlock();
                consumer(p);
                lock();
            }
            unlock();
        }

        /// \brief Default destructor
        ~co_queue() = default;
    };
}

#endif //HAR_CO_QUEUE_HPP
