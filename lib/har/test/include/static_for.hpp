//
// Created by Johannes on 13.06.2020.
//

#ifndef HAR_STATIC_FOR_HPP
#define HAR_STATIC_FOR_HPP

#include <cstddef>

template<std::size_t Start,
        std::size_t End,
        template<std::size_t> typename Func,
        typename... Args>
struct static_for_inner {
    constexpr void operator()(Args... args) {
        Func<Start>()(args...);
        static_for_inner<Start + 1, End, Func, Args...>()(args...);
    }
};

template<std::size_t End,
        template<std::size_t> typename Func,
        typename... Args>
struct static_for_inner<End, End, Func, Args...> {
    constexpr void operator()(Args... args) {

    }
};

template<std::size_t Start,
        std::size_t End,
        template<std::size_t> typename Func,
        typename... Args>
constexpr void static_for(Args... args) {
    static_for_inner<Start, End, Func, Args...>()(args...);
}

#endif //HAR_STATIC_FOR_HPP
