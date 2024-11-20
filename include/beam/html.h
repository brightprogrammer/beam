/// file      : components/base.c
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Generic list implementation

#ifndef BEAM_HTML_H
#define BEAM_HTML_H

// beam
#include <beam/container/list.h>
#include <beam/container/string.h>

///
/// Html is made of smaller html components
/// Basically a list of strings
///
typedef List(String) Html;

///
/// Wrap given html with `before_zstr` and `after_zstr`.
///
/// html[in,out]    : Html to be wrapped.
/// before_zstr[in] : Content to be pushed in the front of provided html.
/// after_zstr[in]  : Content to be pushed after the provided html.
///
/// SUCCESS : `html` wrapped in between `before_zst` <html> `after_zstr`
/// FAILURE : NULL
///
Html *HtmlWrap(Html *html, const char *before_zstr, const char *after_zstr);

#endif // BEAM_HTML_H
