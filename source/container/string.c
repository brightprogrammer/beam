/// file      : container/string.c
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// String implementation

#include <stdarg.h>
#include <stdio.h>

// beam
#include <beam/container/string.h>
#include <beam/log.h>

String* StringInitFmt(String* str, const char* fmt, ...) {
    if(!str || !fmt) {
        LOG_ERROR("invalid arguments");
        return NULL;
    }

    StringClear(str);

    va_list args;
    va_list args_copy;
    va_start(args, fmt);
    va_copy(args_copy, args);

    size_t n = vsnprintf(NULL, 0, fmt, args);
    if(!n) {
        LOG_ERROR("invalid size of final string.");
        return NULL;
    }

    StringResize(str, n + 1);
    vsnprintf(str->data, n + 1, fmt, args_copy);

    str->data[n]  = 0; // null terminate
    str->length  -= 1; // remove length of null-termination

    va_end(args_copy);
    va_end(args);

    return str;
}


String* StringInitCopy(String* dst, String* src) {
    if(!dst || !src) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    VecClear(dst);
    dst->copy_init   = src->copy_init;
    dst->copy_deinit = src->copy_deinit;
    return VecMerge(dst, src);
}


String* StringDeinitCopy(String* copy) {
    if(!copy) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    if(copy->data) {
        memset(copy->data, 0, copy->length);
        free(copy->data);
    }

    memset(copy, 0, sizeof(String));

    return copy;
}
