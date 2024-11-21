/// file      : container/string.c
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// String implementation

#include <beam/container/string.h>
#include <beam/log.h>

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
