/*
 * (C) 2016 Daniel Seither (Kullo GmbH)
 *
 * Distributed under the terms of the Botan license
 */

#if defined __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wcast-align"
    #pragma clang diagnostic ignored "-Wcast-qual"
    #pragma clang diagnostic ignored "-Wconversion"
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    #pragma clang diagnostic ignored "-Wfloat-equal"
    #pragma clang diagnostic ignored "-Wmissing-noreturn"
    #pragma clang diagnostic ignored "-Wmissing-prototypes"
    #pragma clang diagnostic ignored "-Wmissing-variable-declarations"
    #pragma clang diagnostic ignored "-Wparentheses-equality"
    #pragma clang diagnostic ignored "-Wsign-compare"
    #pragma clang diagnostic ignored "-Wsign-conversion"
    #pragma clang diagnostic ignored "-Wundef"
    #pragma clang diagnostic ignored "-Wunreachable-code"
    #pragma clang diagnostic ignored "-Wunreachable-code-break"
    #pragma clang diagnostic ignored "-Wunused-macros"
    #pragma clang diagnostic ignored "-Wunused-function"
    #pragma clang diagnostic ignored "-Wunused-value"

    // Warnings new in clang 3.6
    #if __clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 6)
        #pragma clang diagnostic ignored "-Wreserved-id-macro"
    #endif

    // Warnings new in clang 3.8
    #if __clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 8)
        #pragma clang diagnostic ignored "-Wdouble-promotion"
    #endif
#endif

#include "../sqlite3.c"

#if defined __clang__
    #pragma clang diagnostic pop
#endif

#include "codecext.c"

