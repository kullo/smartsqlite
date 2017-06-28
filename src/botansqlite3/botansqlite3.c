/*
 * (C) 2016 Daniel Seither (Kullo GmbH)
 *
 * Distributed under the terms of the Botan license
 */

#if defined __clang__
    #pragma clang diagnostic push

    // ignore explicitly because clang 3.7 does not support
    // "-Weverything" here.
    #pragma clang diagnostic ignored "-Wcast-align"
    #pragma clang diagnostic ignored "-Wcast-qual"
    #pragma clang diagnostic ignored "-Wconditional-uninitialized"
    #pragma clang diagnostic ignored "-Wconversion"
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
    #pragma clang diagnostic ignored "-Wfloat-equal"
    #pragma clang diagnostic ignored "-Wmissing-variable-declarations"
    #pragma clang diagnostic ignored "-Wsign-compare"
    #pragma clang diagnostic ignored "-Wsign-conversion"
    #pragma clang diagnostic ignored "-Wundef"
    #pragma clang diagnostic ignored "-Wunknown-pragmas"
    #pragma clang diagnostic ignored "-Wunused-function"

    // sufficient for newer clang versions
    #pragma clang diagnostic ignored "-Weverything"
#endif

#include "../sqlite3.c"

#if defined __clang__
    #pragma clang diagnostic pop
#endif

#include "codecext.c"

