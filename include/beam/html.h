/// file      : html.h
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Internal HTML representation implementation

#ifndef BEAM_HTML_H
#define BEAM_HTML_H

#include <stdbool.h>

// beam
#include <beam/container/list.h>
#include <beam/container/string.h>

typedef struct HtmlComponent HtmlComponent;
typedef HtmlComponent *(*HtmlComponentRender)(HtmlComponent *component, void *user_data);

struct HtmlComponent {
    ///
    /// Set to true when something in this component is changed and requires
    /// a re-rendering
    ///
    bool is_changed;

    ///
    /// This html component's render function. Can be used to re-render
    /// this component as well.
    /// If no render method is provided then is_changed and user_data field
    /// is useless and is not used.
    ///
    HtmlComponentRender render;

    ///
    /// User callback data to be passed to render method.
    ///
    void *user_data;

    ///
    /// Html component's string data.
    ///
    String data;
};

HtmlComponent *HtmlComponentInitCopy(HtmlComponent *dst, HtmlComponent *src);
HtmlComponent *HtmlComponentDeinitCopy(HtmlComponent *copy);

///
/// Initialize a new html component with given parameters.
///
/// component[in,out] : Html component to be initialized.
/// render[in]        : Method to be used to (re)render this component.
/// user_data[in]     : User callback data to be passed to `render` method.
///
/// SUCCESS : `component`
/// FAILURE : NULL
///
HtmlComponent *
    HtmlComponentInit(HtmlComponent *component, HtmlComponentRender render, void *user_data);

///
/// Initialize given html object with contents of file at given path.
/// Resets the given html object and stores content in it.
///
/// component[in,out] : Html component to be inited.
/// filepath[in]      : Path where file to be loaded is present.
///
/// SUCCESS : `html`
/// FAILURE : NULL
///
HtmlComponent *HtmlComponentInitFromFile(HtmlComponent *component, const char *filepath);

///
/// Html load from ZString
/// Resets the given html object and stores msg in it.
///
/// component[in,out] : Html component to be inited.
/// zstr[in]          : Null-terminated string to be stored in html.
///
/// SUCCESS : `component`
/// FAILURE : NULL
///
HtmlComponent *HtmlComponentInitFromZStr(HtmlComponent *component, const char *zstr);

///
/// Wrap given html component with `before_zstr` and `after_zstr`.
///
/// component[in,out] : Html component to be wrapped.
/// before_zstr[in]   : Content to be pushed in the front of provided html.
/// after_zstr[in]    : Content to be pushed after the provided html.
///
/// SUCCESS : `component` wrapped in between `before_zstr` <html> `after_zstr`
/// FAILURE : NULL
///
HtmlComponent *
    HtmlComponentWrap(HtmlComponent *component, const char *before_zstr, const char *after_zstr);

///
/// Append given string to given html.
///
/// component[in,out] : Html to append string into.
/// zstr[in]          : Content to be pushed into html
/// len[in]           : Length of given string.
///
/// SUCCESS : `html`
/// FAILURE : NULL
///
HtmlComponent *HtmlComponentAppendCStr(HtmlComponent *component, const char *cstr, size_t len);

///
/// Append given null-terminated string to given html.
///
/// html[in,out] : Html to append string into.
/// zstr[in]     : Content to be pushed into html
/// len[in]      : Length of given string.
///
/// SUCCESS : `component`
/// FAILURE : NULL
///
#define HtmlComponentAppendZStr(html, zstr) HtmlAppendCStr((html), (zstr), strlen((zstr)))


///
/// Prepend given string to given html component.
///
/// component[in,out] : Html to append string into.
/// zstr[in]          : Content to be pushed into html
/// len[in]           : Length of given string.
///
/// SUCCESS : `component`
/// FAILURE : NULL
///
HtmlComponent *HtmlComponentPrependCStr(HtmlComponent *component, const char *cstr, size_t len);

///
/// Prepend given null-terminated string to given html component.
///
/// component[in,out] : Html to append string into.
/// zstr[in]          : Content to be pushed into html
/// len[in]           : Length of given string.
///
/// SUCCESS : `component`
/// FAILURE : NULL
///
#define HtmlComponentPrependZStr(component, zstr)                                                  \
    HtmlComponentPrependCStr((component), (zstr), strlen((zstr)))

///
/// Append a formatted string to given html.
///
/// html[in,out] : Html to append formatted string into.
/// fmt[in]      : Format string, followed by variadic arguments.
///
/// SUCCESS : `html`
/// FAILURE : NULL
///
HtmlComponent *HtmlComponentAppendFmt(HtmlComponent *component, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));


///
/// Prepend a formatted string to given html.
///
/// html[in,out] : Html to append formatted string into.
/// fmt[in]      : Format string, followed by variadic arguments.
///
/// SUCCESS : `html`
/// FAILURE : NULL
///
HtmlComponent *HtmlComponentPrependFmt(HtmlComponent *component, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));

///
/// Html is made of smaller html components
///
typedef Vec(HtmlComponent) Html;

///
/// Initialize given html object with default parameters.
///
/// html[in,out] : Html to be initialized.
///
/// SUCCESS : `html`
/// FAILURE : NULL
///
Html *HtmlInit(Html *html);

///
/// Get complete size of html file by summing up all the components.
///
/// html[in] : Html to get size of.
///
/// RETURN : Total size of given html.
///
size_t HtmlGetCompleteSize(Html *html);

#endif // BEAM_HTML_H
