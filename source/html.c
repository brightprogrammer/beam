/// file      : html.c
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Generic html implementation

#include <beam/html.h>
#include <beam/log.h>

Html *HtmlWrap(Html *html, const char *before_zstr, const char *after_zstr) {
    if(!html || !before_zstr || !after_zstr) {
        LOG_ERROR("inalid arguments");
        return NULL;
    }

    String before;
    StringInitFromZStr(&before, before_zstr);
    String after;
    StringInitFromZStr(&after, after_zstr);

    ListPushFront(html, &before);
    ListPushBack(html, &after);

    return html;
}
