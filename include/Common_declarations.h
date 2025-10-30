#ifndef COMMON_DECLARATIONS_H
#define COMMON_DECLARATIONS_H

#define __STDC_WANT_LIB_EXT1__ 1
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "Colored_printf.h"

#ifdef _DEBUG
#define ON_DEBUG(...) __VA_ARGS__
#else
#define ON_DEBUG(...)
#endif

#define PRINT_LINE() colored_error_printf(RED, BLACK,                                                       \
                                          "Error found, file " __FILE__ ", line %d in \"%s\" function\n",   \
                                          __LINE__, __func__)

#define CLEAR_RESOURCES()   \
do {                        \
    FINAL_CODE              \
} while (false)

#define CHECK_FUNC(func, ...)                       \
do {                                                \
    errno_t err_val = func(__VA_ARGS__);            \
    if (err_val) {                                  \
        PRINT_LINE();                               \
        fprintf_s(stderr, "Code %d: ", err_val);    \
        perror(#func " failed");                    \
        CLEAR_RESOURCES();                          \
        return errno;                               \
    }                                               \
} while (false)

#define MAIN_CHECK_FUNC(func, ...)                  \
do {                                                \
    errno_t err_val = func(__VA_ARGS__);            \
    if (err_val) {                                  \
        PRINT_LINE();                               \
        fprintf_s(stderr, "Code %d: ", err_val);    \
        perror(#func " failed");                    \
        CLEAR_RESOURCES();                          \
        return 0;                                   \
    }                                               \
} while (false)

typedef unsigned char byte_elem_t;

struct Position_info {
    char const *file_name,
               *function_name;
    size_t     line;
};
struct Var_info {
    Position_info position;
    char const    *name;
};

size_t const CANARY = 0XFACE'FACE'FACE'FACE;

#endif
