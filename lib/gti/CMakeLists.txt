cmake_minimum_required(VERSION 3.10)
project("HAR Supplement for GTI"
        LANGUAGES CXX
        VERSION 2.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_PROTO_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_PROTO_FLAGS_RELEASE} -O3 -fno-exceptions")

#region GTI

#endregion
