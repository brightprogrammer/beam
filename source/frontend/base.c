/// file      : frontend/base.c
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Base html for whole website

#include <beam/html.h>
#include <beam/log.h>

Html* HtmlBase(Html* html, const char* msg) {
    if(!html || !msg) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }
    return html;
}
