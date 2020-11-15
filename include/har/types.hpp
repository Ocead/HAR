//
// Created by Johannes on 26.05.2020.
//

#pragma once

#ifndef HAR_TYPES_HPP
#define HAR_TYPES_HPP

#define HAR_DEBUG

#if defined(_MSC_VER)
#define FUNCTION __FUNCSIG__
#elif defined(__GNUC__)
#define FUNCTION __PRETTY_FUNCTION__
#else
#define FUNCTION __func__
#endif

#if defined(NDEBUG) || !defined(HAR_DEBUG)
#define debug(code) (void(0))
#define debug_log(msg) (void(0))
#else

#include <iostream>
#include <string>

#define debug(code) do {\
    code;\
} while(0)

#define debug_log(msg) do {\
    std::clog << ::std::string() + FUNCTION + ": " << msg << ::std::endl;\
} while(0)
#endif

#include <any>
#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace har {

    typedef bool bool_t;                        ///<Boolean type
#if defined(UNICODE)
    typedef wchar_t char_t;                     ///<Character type
#else
    typedef char char_t;                        ///<Character type
#endif
    typedef short int short_t;                  ///<Signed short integer type
    typedef unsigned short int ushort_t;        ///<Unsigned short integer type
    typedef long int int_t;                     ///<Signed integer type
    typedef unsigned long int uint_t;           ///<Unsigned integer type
    typedef double double_t;                    ///<Signed floating point type
    typedef std::basic_string<char_t> string_t; ///<String type
    typedef std::any special_t;                 ///<Any type

    template<typename Key, typename Tp,
            typename Hash = std::hash<Key>,
            typename Pred = std::equal_to<Key>,
            typename Alloc = std::allocator<std::pair<const Key, Tp>>>
    using map = std::unordered_map<Key,
            std::conditional_t<std::is_reference_v<Tp>, std::reference_wrapper<std::remove_reference_t<Tp>>, Tp>,
            Hash, Pred, Alloc>;

    template<typename Tp,
            typename Hash = std::hash<Tp>,
            typename Pred = std::equal_to<Tp>,
            typename Alloc = std::allocator<Tp>>
    using set = std::unordered_set<Tp, Hash, Pred, Alloc>;

    typedef std::size_t participant_h;

    typedef std::basic_istream<char_t> istream;
    typedef std::basic_ostream<char_t> ostream;
    typedef std::basic_stringstream<char_t> stringstream;
    typedef std::basic_ifstream<char_t> ifstream;
    typedef std::basic_ofstream<char_t> ofstream;

    typedef std::basic_string_view<char_t> string_view;

    template<typename CharT, typename Traits = std::char_traits<CharT>>
    class basic_mstreambuf : public std::basic_streambuf<CharT, Traits> {
        using pointer = typename std::add_pointer<CharT>::type;
        using const_pointer = typename std::add_pointer<typename std::add_const<CharT>::type>::type;

    public:
        basic_mstreambuf(const_pointer base, size_t size) {
            pointer ptr(const_cast<pointer>(base));
            this->setg(ptr, ptr, ptr + size);
        }
    };

    typedef basic_mstreambuf<char_t> mstreambuf;

    /// Input stream from memory
    /// \tparam CharT Character type
    /// \tparam Traits Character traits
    template<typename CharT, typename Traits = std::char_traits<CharT>>
    class basic_imstream : virtual basic_mstreambuf<CharT, Traits>, public std::basic_istream<CharT, Traits> {
        using buffer = basic_mstreambuf<CharT, Traits>;
        using stream = std::basic_istream<CharT, Traits>;
        using pointer = typename std::add_pointer<CharT>::type;

    public:
        basic_imstream(pointer base, size_t size) : buffer(base, size), stream(static_cast<buffer *>(this)) {

        }

        explicit basic_imstream(std::basic_string<CharT, Traits> & str) : basic_imstream(str.data(), str.size()) {

        }

        explicit basic_imstream(std::basic_string_view<CharT, Traits> & view) : basic_imstream(view.data(), view.size()) {

        }
    };

    typedef basic_imstream<char_t> imstream;

    using clock = std::conditional_t<
            std::chrono::high_resolution_clock::is_steady,
            std::chrono::high_resolution_clock,
            std::chrono::steady_clock>;

    template<typename Tp>
    inline constexpr auto underlying(Tp e) -> typename std::underlying_type<Tp>::type {
        return static_cast<typename std::underlying_type<Tp>::type>(e);
    }

#include <cctype>

#if defined(UNICODE)
#define text(lit) L##lit

#define remove_r(line) lline.erase(std::remove_if(line.begin(), line.end(), [](auto c) { return std::iswcntrl(c); }), line.end());
#else
#define text(lit) lit

#define remove_r(line) line.erase(std::remove_if((line).begin(), (line).end(), [](auto c) { return std::iscntrl(c); }), (line).end());
#endif

    struct model_info {
        string_t title;
        string_t author;
        string_t version;
        string_t description;
        bool_t editable;
        map<int_t, string_t &> titles;
    };
}

#endif //HAR_TYPES_HPP
