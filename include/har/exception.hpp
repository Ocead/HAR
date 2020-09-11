//
// Created by Johannes on 26.05.2020.
//

#pragma once

#ifndef HAR_EXCEPTION_HPP
#define HAR_EXCEPTION_HPP

#include <exception>
#include <string>
#include <utility>

namespace har {
    namespace exception {
        class exception : public std::exception {
        protected:
            const std::string _source;
            std::string _message;

            exception(std::string source, std::string message) : _source(std::move(source)),
                                                                 _message(std::move(message)) {

            }

        public:
            [[nodiscard]]
            const char * what() const noexcept override {
                return _message.c_str();
            }
        };

        class no_delegate : public exception {
            using exception::exception;

            ~no_delegate() override = default;
        };

        class duplicate_delegate : public exception {
            using exception::exception;

            ~duplicate_delegate() override = default;
        };

        class cell_not_initialized : std::exception {

        };
    }
}

#if defined(__EXCEPTIONS) && defined(NDEBUG)
namespace har {
    [[noreturn]]
    inline void raise(const std::exception & e) {
        throw e;
    }
}
#else

#include <cstring>
#include <csignal>
#include <iostream>

namespace har {
    [[noreturn]]
    inline void raise(const std::exception & e) {
#ifdef __EXCEPTIONS
        throw e;
#else
        if (std::strlen(e.what()) > 0) {
            std::cerr << e.what() << std::endl;
        }
        std::raise(SIGINT);
        std::abort();
#endif
    }
}
#endif

#ifdef __EXCEPTIONS
#define TRY_CATCH(tblock, e, cblock) \
try tblock catch e cblock
#else
#define TRY_CATCH(tblock, e, cblock) \
tblock
#endif

#endif //HAR_EXCEPTION_HPP