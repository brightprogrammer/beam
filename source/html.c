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

Html *HtmlInitFromFile(Html *html, const char *filepath) {
    if(!html || !filepath) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    // temporarily remove copyier
    // so data is not re-duplicated
    GenericCopyInit copier = html->copy_init;
    html->copy_init        = NULL;

    String str = {0};
    if(!ReadCompleteFile(filepath, (void **)&str.data, &str.length, &str.capacity)) {
        LOG_ERROR("failed to read file.");
        html->copy_init = copier;
        return NULL;
    }

    // when inserted it'll just be memcopied
    ListPushBack(html, &str);

    // switch it up!
    html->copy_init = copier;
    return html;
}


Html *HtmlInitFromZStr(Html *html, const char *msg) {
    if(!html || !msg) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    // remove any previous data
    ListInit(html, StringInitCopy, StringDeinitCopy);

    String str;
    TempStringFromZStr(&str, msg);
    ListPushBack(html, &str);
    memset(&str, 0, sizeof(String));

    return html;
}


Html *HtmlWrap(Html *html, const char *before_zstr, const char *after_zstr) {
    if(!html || !before_zstr || !after_zstr) {
        LOG_ERROR("inalid arguments");
        return NULL;
    }

    String data[2];
    TempStringFromZStr(data, before_zstr);
    TempStringFromZStr(data + 1, after_zstr);
    ListPushFront(html, data);
    ListPushBack(html, data + 1);
    memset(data, 0, sizeof(data));

    return html;
}


Html *HtmlAppendCStr(Html *html, const char *cstr, size_t len) {
    if(!html || !cstr || !len) {
        LOG_ERROR("invalid arguments");
        return NULL;
    }

    String str = {0};
    TempStringFromCStr(&str, cstr, len);
    ListPushBack(html, &str);
    memset(&str, 0, sizeof(String));

    return html;
}


Html *HtmlPrependCStr(Html *html, const char *cstr, size_t len) {
    if(!html || !cstr || !len) {
        LOG_ERROR("invalid arguments");
        return NULL;
    }

    String str = {0};
    TempStringFromCStr(&str, cstr, len);
    ListPushFront(html, &str);
    memset(&str, 0, sizeof(String));

    return html;
}


Html *HtmlAppendFmt(Html *html, const char *fmt, ...) {
    if(!html || !fmt) {
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
    HtmlAppendCStr(html, data, n);
    memset(data, 0, n + 1);
    free(data);

    va_end(args_copy);
    va_end(args);

    return html;
}


Html *HtmlPrependFmt(Html *html, const char *fmt, ...) {
    if(!html || !fmt) {
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
    HtmlPrependCStr(html, data, n);
    memset(data, 0, n + 1);
    free(data);

    va_end(args_copy);
    va_end(args);

    return html;
}


size_t HtmlGetCompleteSize(Html *html) {
    if(!html) {
        LOG_ERROR("invalid arguments.");
        return 0;
    }

    size_t  total_size = 0;
    String *str;
    size_t  iter;
    ListForeachPtr(html, str, iter) {
        total_size += str->length;
    }

    return total_size;
}
