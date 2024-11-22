/// file      : html.h
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Internal HTML representation implementation

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
/// Initialize given html object with contents of file at given path.
/// Resets the given html object and stores content in it.
///
/// html[in,out] : Html to be inited.
/// filepath[in] : Path where file to be loaded is present.
///
/// SUCCESS : `html`
/// FAILURE : NULL
///
Html *HtmlInitFromFile(Html *html, const char *filepath);

///
/// Html load from ZString
/// Resets the given html object and stores msg in it.
///
/// html[in,out] : Html to be inited.
/// msg[in]      : Null-terminated string to be stored in html.
///
/// SUCCESS : `html`
/// FAILURE : NULL
///
Html *HtmlInitFromZStr(Html *html, const char *msg);

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

///
/// Append given string to given html.
///
/// html[in,out] : Html to append string into.
/// zstr[in]     : Content to be pushed into html
/// len[in]      : Length of given string.
///
/// SUCCESS : `html`
/// FAILURE : NULL
///
Html *HtmlAppendCStr(Html *html, const char *cstr, size_t len);

///
/// Append given null-terminated string to given html.
///
/// html[in,out] : Html to append string into.
/// zstr[in]     : Content to be pushed into html
/// len[in]      : Length of given string.
///
/// SUCCESS : `html`
/// FAILURE : NULL
///
#define HtmlAppendZStr(html, zstr) HtmlAppendCStr((html), (zstr), strlen((zstr)))

///
/// Append a formatted string to given html.
///
/// html[in,out] : Html to append formatted string into.
/// fmt[in]      : Format string, followed by variadic arguments.
///
/// SUCCESS : `html`
/// FAILURE : NULL
///
Html *HtmlAppendFmt(Html *html, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

///
/// Get complete size of html file by summing up all the components.
///
/// html[in] : Html to get size of.
///
/// RETURN : Total size of given html.
///
size_t HtmlGetCompleteSize(Html *html);

#endif // BEAM_HTML_H
