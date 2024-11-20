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

    if(!src->data || !src->length) {
        LOG_ERROR("invalid string provided to create copy from.");
        return NULL;
    }

    if(dst->data) {
        memset(dst->data, 0, dst->length);
        free(dst->data);
    }

    dst->data        = NULL;
    dst->length      = 0;
    dst->capacity    = 0;
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

    return copy;
}
