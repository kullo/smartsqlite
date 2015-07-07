cmake_minimum_required(VERSION 2.8)
project(smartsqlite)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g")
    set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -g ${CMAKE_CXX_FLAGS}")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O2 -g")
    set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -g ${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wformat=2 -Wlogical-op -Wmissing-include-dirs -Wnon-virtual-dtor -Woverloaded-virtual -Wswitch-default -Wuninitialized")

    # Hardening
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector -pie -fPIE -Wformat -Wformat-security -D_FORTIFY_SOURCE=2 -Wl,-z,relro,-z,now")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector -pie -fPIE -Wformat -Wformat-security -D_FORTIFY_SOURCE=2 -Wl,-z,relro,-z,now")

# Workaround for CMake < 3.1: Variables such as 'MSVC' are expanded in 'if'
# statements even if they are quoted. We can't enable the policy CMP0054 because
# we need to support CMake 2.8. Therefore, we append a space on both sides,
# which disables automatic expansion.
elseif ("${CMAKE_CXX_COMPILER_ID} " STREQUAL "MSVC ")
    # use shared MSVC++ runtime, see http://www.cmake.org/Wiki/CMake_FAQ#Dynamic_Replace
    foreach(flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
       if(${flag_var} MATCHES "/MT")
          string(REGEX REPLACE "/MT" "/MD" ${flag_var} "${${flag_var}}")
       endif()
    endforeach()
    # force gtest to use shared MSVC++ runtime
    set(gtest_force_shared_crt ON)
endif()

if(APPLE)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -mmacosx-version-min=10.9")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.9 -stdlib=libc++")
endif()

# Google Mock / Google Test
add_subdirectory("lib/gmock")
include_directories("${gmock_SOURCE_DIR}/include" "${gtest_SOURCE_DIR}/include")

# smartsqlite
add_subdirectory("src")
include_directories("include")

# Begin: Copy smartsqlite headers into build (called 'binary' in CMAKE slang) directory
file(GLOB_RECURSE SMARTSQLITE_HEADERS_RELATIVE RELATIVE
     "${CMAKE_SOURCE_DIR}/include/smartsqlite"
     "include/smartsqlite/*.h")

set(SMARTSQLITE_HEADERS_BINARY "")
foreach (H ${SMARTSQLITE_HEADERS_RELATIVE})
    set(HEADER_SRC ${CMAKE_SOURCE_DIR}/include/smartsqlite/${H})
    set(HEADER_BIN ${CMAKE_BINARY_DIR}/include/smartsqlite/${H})
    LIST(APPEND SMARTSQLITE_HEADERS_BINARY ${HEADER_BIN})

    add_custom_command(OUTPUT ${HEADER_BIN}
                       COMMAND cmake -E copy_if_different ${HEADER_SRC} ${HEADER_BIN}
                       DEPENDS ${HEADER_SRC})
endforeach()

add_custom_target(copy_headers_smartsqlite ALL DEPENDS ${SMARTSQLITE_HEADERS_BINARY})
# End: Copy smartsqlite headers

# tests
add_subdirectory("test")

enable_testing()
add_test(all_tests test/${PROJECT_NAME}_tests)
