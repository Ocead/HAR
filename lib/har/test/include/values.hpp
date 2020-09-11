//
// Created by Johannes on 10.06.2020.
//

#ifndef HAR_VALUES_HPP
#define HAR_VALUES_HPP

#include <random>
#include <sstream>

#include <har/value.hpp>

namespace har {

    static std::random_device rd;
    static std::mt19937_64 gen(rd());

    template<typename T>
    inline T random_value() noexcept;

    template<>
    inline bool_t random_value<bool_t>() noexcept {
        std::bernoulli_distribution bd{ 0.5 };
        return bd(gen);
    }

    template<>
    inline int_t random_value<int_t>() noexcept {
        return gen();
    }

    template<>
    inline uint_t random_value<uint_t>() noexcept {
        return gen();
    }

    template<>
    inline double_t random_value<double_t>() noexcept {
        return double_t(gen()) / double_t(gen());
    }

    template<>
    inline string_t random_value<string_t>() noexcept {
        char_t str[20]{ 0 };
        for (auto & i : str)
            i = gen();
        return string_t(str, 20);
    }

    template<>
    inline ccoords_t random_value<ccoords_t>() noexcept {
        return ccoords_t(random_value<double_t>(), random_value<double_t>());
    }

    template<>
    inline dcoords_t random_value<dcoords_t>() noexcept {
        return dcoords_t(random_value<int_t>(), random_value<int_t>());
    }

    template<>
    inline direction_t random_value<direction_t>() noexcept {
        return direction_t(rd());
    }

    template<>
    inline color_t random_value<color_t>() noexcept {
        return color_t(random_value<uint_t>(), random_value<uint_t>(), random_value<uint_t>(), random_value<uint_t>());
    }

    template<>
    inline special_t random_value<special_t>() noexcept {
        return std::make_any<uint_t>(random_value<uint_t>());
    }

    template<>
    inline callback_t random_value<callback_t>() noexcept {
        auto val = random_value<int_t>();
        return [=](cell &) { return val; };
    }

    template<>
    inline part_h random_value<part_h>() noexcept {
        return PART[gen()];
    }

}

#endif //HAR_VALUES_HPP
