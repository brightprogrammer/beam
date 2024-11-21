/// file      : frontend/base.c
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Base html for whole website

// beam
#include "frontend.h"
#include <beam/log.h>

Html* WrapContent(Html* html) {
    if(!html) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    const char* before =
        "<center>"
        "<h1>";

    const char* after =
        "</h1>"
        "</center>";

    if(!HtmlWrap(html, before, after)) {
        LOG_ERROR("failed to wrap html.");
        return NULL;
    }

    return html;
}


Html* WrapBase(Html* html) {
    if(!html) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    const char* before =
        "<!doctype html><html>"
        "<head>"
        "<title>beam</title>"
        "<meta name=\"color-scheme\" content=\"dark\">"
        "</head>"
        "<body>";

    const char* after =
        "</body>"
        "</html>";

    if(!HtmlWrap(html, before, after)) {
        LOG_ERROR("failed to wrap html.");
        return NULL;
    }

    return html;
}
