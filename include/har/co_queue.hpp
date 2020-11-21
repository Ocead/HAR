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
        mutable std::atomic<unsigned> _quewait; ///<
        mutable std::mutex _queex; ///<
        std::deque<T> _queue; ///<

        static_assert(decltype(_quewait)::is_always_lock_free, "Atomic counter is not lock-free!");

        ///
        void lock() const {
            _queex.lock();
        }

        ///
        void unlock() const {
            _queex.unlock();
        }

    public:

        /// \brief Constructor
        co_queue() : _quewait(0u),
                     _queex(),
                     _queue() {

        }

        ///
        /// \return
        [[nodiscard]]
        bool empty() const {
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

        /// Pushes all elements in a range onto the queue
        /// \tparam Tp Iterator type
        /// \param begin Begin iterator
        /// \param end End iterator
        template<typename Tp>
        void push(Tp & begin, Tp & end) {
            lock();
            for (; begin != end; begin++) {
                _queue.push_front(*begin);
            }
            unlock();
        }

        ///
        /// \return
        T pop() {
            lock();
            T t{ std::move(_queue.back()) };
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

        ///
        /// \param consumer
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

        /// 
        /// \param consumer
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
