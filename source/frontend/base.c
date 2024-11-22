/// file      : frontend/base.c
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Base html for whole website

// beam
#include "frontend.h"
#include <beam/file.h>
#include <beam/log.h>

Html* WrapContent(Html* html) {
    if(!html) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    if(!HtmlWrap(html, "<center>", "</center>")) {
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
        "<style>"
        "    /* General page styling */"
        "    body {"
        "        width: 100%;"
        "        font-family: Arial, sans-serif;"
        "        margin: 0;"
        "        padding: 0;"
        "        display: flex;"
        "        justify-content: center;"
        "        align-items: center;"
        "        height: 100vh;"
        "        background-color: #f4f4f4;"
        "        color: #333;"
        "    }"
        "    .container {"
        "        text-align: center;"
        "        padding: 20px;"
        "        background: #fff;"
        "        border-radius: 8px;"
        "        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);"
        "    }"

        "    table {"
        "        width: 100%;"
        "        border-collapse: collapse;"
        "        margin: 20px 0;"
        "        font-size: 16px;"
        "        text-align: left;"
        "    }"
        "    th, td {"
        "        padding: 10px;"
        "        border: 1px solid #ddd;"
        "    }"
        "    th {"
        "        background-color: #f4f4f4;"
        "    }"
        "    tr:nth-child(even) {"
        "        background-color: #f9f9f9;"
        "    }"

        "    /* General link styles */"
        "    a {"
        "        text-decoration: none; /* Remove underline */"
        "        color: #0077cc; /* Default color */"
        "        font-weight: bold;"
        "        transition: color 0.3s ease, border-bottom 0.3s ease; /* Smooth animations */"
        "        border-bottom: 2px solid transparent; /* Underline effect */"
        "    }"
        "    /* Hover effect */"
        "    a:hover {"
        "        color: #005fa3; /* Darker shade on hover */"
        "        border-bottom: 2px solid #005fa3; /* Add underline on hover */"
        "    }"
        "    /* Focus effect for accessibility */"
        "    a:focus {"
        "        outline: 3px dashed #ffcc00; /* Highlighted outline for focus */"
        "        outline-offset: 2px;"
        "    }"
        "    /* Visited link style */"
        "    a:visited {"
        "        color: #7a5a9a; /* Dimmed color for visited links */"
        "    }"

        "    .codebox {"
        "        /* Below are styles for the codebox (not the code itself) */"
        "        text-align: left;"
        "        border:1px solid black;"
        "        background-color:#f9f9f9;"
        "        height:90vh;"
        "        width:90vw;"
        "        overflow:auto;    "
        "        padding:10px;"
        "    }"
        "    .codebox code {"
        "        /* Styles in here affect the text of the codebox */"
        "        font-size:0.9em;"
        "    }"

        "</style>"

        "<link rel=\"stylesheet\" "
        "href=\"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.8.0/styles/"
        "default.min.css\">"

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


Html* WrapDirEntryInTable(Html* html, DirEntry* entry) {
    if(!html || !entry) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    HtmlAppendFmt(
        html,
        "<tr>"
        "<th><a href=\"%s/\">%s</a></th>"
        "<th>%s</th>"
        "</tr>",
        entry->name.data,
        entry->name.data,
        DirEntryTypeToZStr(entry->type)
    );

    return html;
}


Html* WrapDirContents(Html* html, DirContents* dir_contents) {
    if(!html || !dir_contents) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    HtmlAppendFmt(
        html,
        "<tr>"
        "<th>Name</th>"
        "<th>Type</th>"
        "</tr>"
    );

    DirEntry* entry = NULL;
    size_t    i     = 0;
    VecForeachPtr(dir_contents, entry, i) {
        WrapDirEntryInTable(html, entry);
    }

    HtmlWrap(html, "<table>", "</table>");

    return html;
}


Html* Wrap404(Html* html) {
    if(!html) {
        return NULL;
    }

    return HtmlAppendZStr(
        html,
        " <h1>404</h1>"
        " <p>Oops! The page you're looking for can't be found.</p>"
        " <p>Let's get you back to safety:</p>"
        " <a href=\"/\">Go to Homepage</a>"
    );
}


Html* WrapFileContent(Html* html, const char* filepath) {
    if(!html || !filepath) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    // TODO: sending 404 this way is not the best thing to do
    // we need to send 404 in response code as well
    if(GetFileSize(filepath) > 1024 * 1024) {
        LOG_ERROR("big file requested, returned 404");
        Wrap404(html);
        return html;
    }

    String file = {0};
    if(!ReadCompleteFile(filepath, (void**)&file.data, &file.length, &file.capacity)) {
        LOG_ERROR("failed to read file.");
        return NULL;
    }

    const char* language = NULL;
    if(strstr(filepath, ".c") || strstr(filepath, ".h")) {
        language = "langauge-c";
    } else if(strstr(filepath, ".cpp") || strstr(filepath, ".hpp")) {
        language = "language-cpp";
    } else if(strstr(filepath, "CMakeLists.txt")) {
        language = "language-cmake";
    } else if(strstr(filepath, ".md")) {
        language = "language-markdown";
    } else {
        language = "langauge-text";
    }

    html = HtmlAppendFmt(
        html,
        "<div class=\"codebox\"><pre><code class=\"%s\">%s</code></pre></div>",
        language,
        file.data
    );
    StringDeinit(&file);

    return html;
}
