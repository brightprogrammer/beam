/// file      : html.c
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Generic html implementation

#include <stdarg.h>

// beam
#include <beam/html.h>
#include <beam/log.h>
#include <beam/file.h>

HtmlComponent *HtmlComponentInitCopy(HtmlComponent *dst, HtmlComponent *src) {
    if(!dst || !src) {
        LOG_ERROR("invalid arguments");
        return NULL;
    }

    if(!StringInitCopy(&dst->data, &src->data)) {
        LOG_ERROR("failed to create string copy.");
        return NULL;
    }

    dst->render     = src->render;
    dst->user_data  = src->user_data;
    dst->is_changed = src->is_changed;

    return dst;
}


HtmlComponent *HtmlComponentDeinitCopy(HtmlComponent *copy) {
    if(!copy) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    StringDeinitCopy(&copy->data);
    memset(copy, 0, sizeof(HtmlComponent));

    return copy;
}


HtmlComponent *
    HtmlComponentInit(HtmlComponent *component, HtmlComponentRender render, void *user_data) {
    if(!component) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    HtmlComponentDeinitCopy(component);

    // try to render for the first time
    if(!render(component, user_data)) {
        LOG_ERROR("html component render failed.");
        return NULL;
    }

    component->render     = render;
    component->user_data  = user_data;
    component->is_changed = false;

    return component;
}


HtmlComponent *HtmlInitFromFile(HtmlComponent *component, const char *filepath) {
    if(!component || !filepath) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    // components initialized like don't have to re-render
    // so is_changed, render and user_data fields are redundant.
    HtmlComponentDeinitCopy(component);

    // read whole file into component data
    if(!ReadCompleteFile(
           filepath,
           (void **)&component->data.data,
           &component->data.length,
           &component->data.capacity
       )) {
        LOG_ERROR("failed to read file.");
        return NULL;
    }

    return component;
}


HtmlComponent *HtmlComponentInitFromZStr(HtmlComponent *component, const char *zstr) {
    if(!component || !zstr) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    // components initialized like don't have to re-render
    // so is_changed, render and user_data fields are redundant.
    HtmlComponentDeinitCopy(component);

    StringPushBackZStr(&component->data, zstr);


    return component;
}


HtmlComponent *
    HtmlComponentWrap(HtmlComponent *component, const char *before_zstr, const char *after_zstr) {
    if(!component || !before_zstr || !after_zstr) {
        LOG_ERROR("inalid arguments");
        return NULL;
    }

    StringPushFrontZStr(&component->data, before_zstr);
    StringPushFrontZStr(&component->data, after_zstr);

    return component;
}


HtmlComponent *HtmlAppendCStr(HtmlComponent *component, const char *cstr, size_t len) {
    if(!component || !cstr || !len) {
        LOG_ERROR("invalid arguments");
        return NULL;
    }

    StringPushBackCStr(&component->data, cstr, len);

    return component;
}


HtmlComponent *HtmlPrependCStr(HtmlComponent *component, const char *cstr, size_t len) {
    if(!component || !cstr || !len) {
        LOG_ERROR("invalid arguments");
        return NULL;
    }

    StringPushFrontCStr(&component->data, cstr, len);

    return component;
}


HtmlComponent *HtmlComponentAppendFmt(HtmlComponent *component, const char *fmt, ...) {
    if(!component || !fmt) {
        LOG_ERROR("invalid arguments");
        return NULL;
    }

    va_list args;
    va_list args_copy;
    va_start(args, fmt);
    va_copy(args_copy, args);

    size_t n    = vsnprintf(NULL, 0, fmt, args);
    char  *data = malloc(n + 1);
    if(!data) {
        LOG_ERROR("malloc() failed : %s.", strerror(errno));
        return NULL;
    }
    data[n] = 0;

    vsnprintf(data, n + 1, fmt, args_copy);
    StringPushBackCStr(&component->data, data, n);
    memset(data, 0, n + 1);
    free(data);

    va_end(args_copy);
    va_end(args);

    return component;
}


HtmlComponent *HtmlComponentPrependFmt(HtmlComponent *component, const char *fmt, ...) {
    if(!component || !fmt) {
        LOG_ERROR("invalid arguments");
        return NULL;
    }

    va_list args;
    va_list args_copy;
    va_start(args, fmt);
    va_copy(args_copy, args);

    size_t n    = vsnprintf(NULL, 0, fmt, args);
    char  *data = malloc(n + 1);
    if(!data) {
        LOG_ERROR("malloc() failed : %s.", strerror(errno));
        return NULL;
    }
    data[n] = 0;

    vsnprintf(data, n + 1, fmt, args_copy);
    StringPushFrontCStr(&component->data, data, n);
    memset(data, 0, n + 1);
    free(data);

    va_end(args_copy);
    va_end(args);

    return component;
}


Html *HtmlInit(Html *html) {
    if(!html) {
        LOG_ERROR("invalid arguments");
        return NULL;
    }

    VecInit(html, HtmlComponentInitCopy, HtmlComponentDeinitCopy);

    return html;
}


size_t HtmlGetCompleteSize(Html *html) {
    if(!html) {
        LOG_ERROR("invalid arguments.");
        return 0;
    }

    size_t         total_size = 0;
    HtmlComponent *component;
    size_t         iter;
    VecForeachPtr(html, component, iter) {
        total_size += component->data.length;
    }

    return total_size;
}
