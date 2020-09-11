//
// Created by Johannes on 10.06.2020.
//

#pragma once

#ifndef HAR_PLATFORM_HPP
#define HAR_PLATFORM_HPP

#if defined(_WIN32) || defined(_WIN64)
#if defined(_WIN64)
#define ENV64
#else
#define ENV32
#endif
#elif defined(__unix__)
#if defined(__x86_64__) || defined(__ppc64__)
#define ENV64
#else
#define ENV32
#endif
#endif

#endif //HAR_PLATFORM_HPP
