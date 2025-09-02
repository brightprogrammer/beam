/// file      : http.c
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Provide HTTP constructs for beam.

// socket
#include <arpa/inet.h>

#include <Misra.h>
#include <Beam/Http.h>

void HttpHeaderDeinit(HttpHeader *header) {
    if(!header) {
        LOG_FATAL("Invalid arguments");
    }

    StrDeinit(&header->key);
    StrDeinit(&header->value);
}

HttpHeader *HttpHeadersFind(HttpHeaders *headers, const char *key) {
    if(!headers || !key) {
        LOG_ERROR("Invalid arguments.");
        return NULL;
    }

    VecForeachPtr(headers, header, {
        if(0 == ZstrCompare(header->key.data, key)) {
            return header;
        }
    });

    return NULL;
}

HttpRequestMethod http_request_method_from_str(Str *mstr) {
    if(!mstr || !mstr->data) {
        LOG_FATAL("Invalid arguments");
    }

    if(ZstrCompareN(mstr->data, "GET", 3)) {
        return HTTP_REQUEST_METHOD_GET;
    } else if(ZstrCompareN(mstr->data, "POST", 4)) {
        return HTTP_REQUEST_METHOD_POST;
    } else if(ZstrCompareN(mstr->data, "DELETE", 6)) {
        return HTTP_REQUEST_METHOD_DELETE;
    } else if(ZstrCompareN(mstr->data, "PUT", 3)) {
        return HTTP_REQUEST_METHOD_PUT;
    } else if(ZstrCompareN(mstr->data, "PATCH", 5)) {
        return HTTP_REQUEST_METHOD_PATCH;
    } else if(ZstrCompareN(mstr->data, "HEAD", 4)) {
        return HTTP_REQUEST_METHOD_HEAD;
    } else if(ZstrCompareN(mstr->data, "OPTIONS", 7)) {
        return HTTP_REQUEST_METHOD_OPTIONS;
    } else if(ZstrCompareN(mstr->data, "CONNECT", 7)) {
        return HTTP_REQUEST_METHOD_CONNECT;
    } else if(ZstrCompareN(mstr->data, "TRACE", 5)) {
        return HTTP_REQUEST_METHOD_TRACE;
    }

    return HTTP_REQUEST_METHOD_UNKNOWN;
}

const char *HttpRequestParse(HttpRequest *req, const char *in) {
    if(!req || !in) {
        LOG_FATAL("Invalid arguments");
    }

    *req = HttpRequestInit();

    const char* out = in;

    Str method = StrInit(), version = StrInit();
    StrReadFmt(in, "{} {} {}\r\n", method, req->url, version);

    if(out == in) {
        LOG_ERROR("Http request parse failed. Not in valid format.");
        return in;
    }

    // make sure http version is good
    if(0 != ZstrCompareN(version.data, "HTTP/1.1", 8)) {
        StrDeinit(&version);
        LOG_ERROR("Invalid/Unsupported http verison.");
        return in;
    }
    StrDeinit(&version);

    // parse method and verify
    req->method = http_request_method_from_str(&method);
    StrDeinit(&method);
    if(req->method == HTTP_REQUEST_METHOD_UNKNOWN) {
        LOG_ERROR("Invalid http request method.");
        return in;
    }

    HttpHeader  hh  = HttpHeaderInit();

    // Init headers vector to take ownership of items
    req->headers = VecInitWithDeepCopyT(req->headers, NULL, HttpHeaderDeinit);

    while(true) {
        // we stop parsing if we don't make any progress
        // i.e "out == in"
        out = in;

        // if we get CRLF at the beginning, then there are no headers
        if(0 == ZstrCompareN(in, "\r\n", 2)) {
            out = in = in + 2;
            break;
        }

        // this was memset after last push back
        hh = HttpHeaderInit();

        // get header key
        StrReadFmt(in, "{}: {}\r\n", hh.key, hh.value);
        if(out == in) {
            LOG_ERROR("Failed to find header key. Invalid http request.");
            HttpRequestDeinit(req);
            return in;
        }

        VecPushBack(&req->headers, hh);
    }

    return out;
}


void HttpRequestDeinit(HttpRequest *request) {
    if(!request) {
        LOG_FATAL("invalid arguments");
    }

    StrDeinit(&request->url);
    VecDeinit(&request->headers);
    request->method = HTTP_REQUEST_METHOD_UNKNOWN;
}


const char *HttpResponseCodeToZstr(HttpResponseCode code) {
    switch(code) {
        case HTTP_RESPONSE_CODE_CONTINUE :
            return "100 Continue";
        case HTTP_RESPONSE_CODE_SWITCHING_PROTOCOLS :
            return "101 Switching Protocols";
        case HTTP_RESPONSE_CODE_PROCESSING :
            return "102 Processing";
        case HTTP_RESPONSE_CODE_EARLY_HINTS :
            return "103 Early Hints";
        case HTTP_RESPONSE_CODE_OK :
            return "200 OK";
        case HTTP_RESPONSE_CODE_CREATED :
            return "201 Created";
        case HTTP_RESPONSE_CODE_ACCEPTED :
            return "202 Accepted";
        case HTTP_RESPONSE_CODE_NON_AUTHORITATIVE_INFORMATION :
            return "203 Non-Authoritative Information";
        case HTTP_RESPONSE_CODE_NO_CONTENT :
            return "204 No Content";
        case HTTP_RESPONSE_CODE_RESET_CONTENT :
            return "205 Reset Content";
        case HTTP_RESPONSE_CODE_PARTIAL_CONTENT :
            return "206 Partial Content";
        case HTTP_RESPONSE_CODE_MULTI_STATUS :
            return "207 Multi-Status";
        case HTTP_RESPONSE_CODE_ALREADY_REPORTED :
            return "208 Already Reported";
        case HTTP_RESPONSE_CODE_IM_USED :
            return "226 IM Used";
        case HTTP_RESPONSE_CODE_MULTIPLE_CHOICES :
            return "300 Multiple Choices";
        case HTTP_RESPONSE_CODE_MOVED_PERMANENTLY :
            return "301 Moved Permanently";
        case HTTP_RESPONSE_CODE_FOUND :
            return "302 Found";
        case HTTP_RESPONSE_CODE_SEE_OTHER :
            return "303 See Other";
        case HTTP_RESPONSE_CODE_NOT_MODIFIED :
            return "304 Not Modified";
        case HTTP_RESPONSE_CODE_USE_PROXY :
            return "305 Use Proxy";
        case HTTP_RESPONSE_CODE_TEMPORARY_REDIRECT :
            return "307 Temporary Redirect";
        case HTTP_RESPONSE_CODE_PERMANENT_REDIRECT :
            return "308 Permanent Redirect";
        case HTTP_RESPONSE_CODE_BAD_REQUEST :
            return "400 Bad Request";
        case HTTP_RESPONSE_CODE_UNAUTHORIZED :
            return "401 Unauthorized";
        case HTTP_RESPONSE_CODE_PAYMENT_REQUIRED :
            return "402 Payment Required";
        case HTTP_RESPONSE_CODE_FORBIDDEN :
            return "403 Forbidden";
        case HTTP_RESPONSE_CODE_NOT_FOUND :
            return "404 Not Found";
        case HTTP_RESPONSE_CODE_METHOD_NOT_ALLOWED :
            return "405 Method Not Allowed";
        case HTTP_RESPONSE_CODE_NOT_ACCEPTABLE :
            return "406 Not Acceptable";
        case HTTP_RESPONSE_CODE_PROXY_AUTHENTICATION_REQUIRED :
            return "407 Proxy Authentication Required";
        case HTTP_RESPONSE_CODE_REQUEST_TIMEOUT :
            return "408 Request Timeout";
        case HTTP_RESPONSE_CODE_CONFLICT :
            return "409 Conflict";
        case HTTP_RESPONSE_CODE_GONE :
            return "410 Gone";
        case HTTP_RESPONSE_CODE_LENGTH_REQUIRED :
            return "411 Length Required";
        case HTTP_RESPONSE_CODE_PRECONDITION_FAILED :
            return "412 Precondition Failed";
        case HTTP_RESPONSE_CODE_PAYLOAD_TOO_LARGE :
            return "413 Payload Too Large";
        case HTTP_RESPONSE_CODE_URI_TOO_LONG :
            return "414 URI Too Long";
        case HTTP_RESPONSE_CODE_UNSUPPORTED_MEDIA_TYPE :
            return "415 Unsupported Media Type";
        case HTTP_RESPONSE_CODE_RANGE_NOT_SATISFIABLE :
            return "416 Range Not Satisfiable";
        case HTTP_RESPONSE_CODE_EXPECTATION_FAILED :
            return "417 Expectation Failed";
        case HTTP_RESPONSE_CODE_IM_A_TEAPOT :
            return "418 I'm a teapot";
        case HTTP_RESPONSE_CODE_MISDIRECTED_REQUEST :
            return "421 Misdirected Request";
        case HTTP_RESPONSE_CODE_UNPROCESSABLE_ENTITY :
            return "422 Unprocessable Entity";
        case HTTP_RESPONSE_CODE_LOCKED :
            return "423 Locked";
        case HTTP_RESPONSE_CODE_FAILED_DEPENDENCY :
            return "424 Failed Dependency";
        case HTTP_RESPONSE_CODE_TOO_EARLY :
            return "425 Too Early";
        case HTTP_RESPONSE_CODE_UPGRADE_REQUIRED :
            return "426 Upgrade Required";
        case HTTP_RESPONSE_CODE_PRECONDITION_REQUIRED :
            return "428 Precondition Required";
        case HTTP_RESPONSE_CODE_TOO_MANY_REQUESTS :
            return "429 Too Many Requests";
        case HTTP_RESPONSE_CODE_REQUEST_HEADER_FIELDS_TOO_LARGE :
            return "431 Request Header Fields Too Large";
        case HTTP_RESPONSE_CODE_UNAVAILABLE_FOR_LEGAL_REASONS :
            return "451 Unavailable For Legal Reasons";
        case HTTP_RESPONSE_CODE_INTERNAL_SERVER_ERROR :
            return "500 Internal Server Error";
        case HTTP_RESPONSE_CODE_NOT_IMPLEMENTED :
            return "501 Not Implemented";
        case HTTP_RESPONSE_CODE_BAD_GATEWAY :
            return "502 Bad Gateway";
        case HTTP_RESPONSE_CODE_SERVICE_UNAVAILABLE :
            return "503 Service Unavailable";
        case HTTP_RESPONSE_CODE_GATEWAY_TIMEOUT :
            return "504 Gateway Timeout";
        case HTTP_RESPONSE_CODE_HTTP_VERSION_NOT_SUPPORTED :
            return "505 HTTP Version Not Supported";
        case HTTP_RESPONSE_CODE_VARIANT_ALSO_NEGOTIATES :
            return "506 Variant Also Negotiates";
        case HTTP_RESPONSE_CODE_INSUFFICIENT_STORAGE :
            return "507 Insufficient Storage";
        case HTTP_RESPONSE_CODE_LOOP_DETECTED :
            return "508 Loop Detected";
        case HTTP_RESPONSE_CODE_NOT_EXTENDED :
            return "510 Not Extended";
        case HTTP_RESPONSE_CODE_NETWORK_AUTHENTICATION_REQUIRED :
            return "511 Network Authentication Required";
        default :
            return NULL;
    }
}


const char *HttpContentTypeToZstr(HttpContentType type) {
    switch(type) {
        case HTTP_CONTENT_TYPE_TEXT_PLAIN :
            return "text/plain";
        case HTTP_CONTENT_TYPE_TEXT_HTML :
            return "text/html";
        case HTTP_CONTENT_TYPE_TEXT_CSS :
            return "text/css";
        case HTTP_CONTENT_TYPE_TEXT_JAVASCRIPT :
            return "text/javascript";
        case HTTP_CONTENT_TYPE_APPLICATION_JSON :
            return "application/json";
        case HTTP_CONTENT_TYPE_APPLICATION_XML :
            return "application/xml";
        case HTTP_CONTENT_TYPE_APPLICATION_JAVASCRIPT :
            return "application/javascript";
        case HTTP_CONTENT_TYPE_APPLICATION_PDF :
            return "application/pdf";
        case HTTP_CONTENT_TYPE_APPLICATION_ZIP :
            return "application/zip";
        case HTTP_CONTENT_TYPE_APPLICATION_OCTET_STREAM :
            return "application/octet-stream";
        case HTTP_CONTENT_TYPE_IMAGE_JPEG :
            return "image/jpeg";
        case HTTP_CONTENT_TYPE_IMAGE_PNG :
            return "image/png";
        case HTTP_CONTENT_TYPE_IMAGE_GIF :
            return "image/gif";
        case HTTP_CONTENT_TYPE_IMAGE_BMP :
            return "image/bmp";
        case HTTP_CONTENT_TYPE_IMAGE_SVG_XML :
            return "image/svg+xml";
        case HTTP_CONTENT_TYPE_AUDIO_MPEG :
            return "audio/mpeg";
        case HTTP_CONTENT_TYPE_AUDIO_OGG :
            return "audio/ogg";
        case HTTP_CONTENT_TYPE_AUDIO_WAV :
            return "audio/wav";
        case HTTP_CONTENT_TYPE_VIDEO_MP4 :
            return "video/mp4";
        case HTTP_CONTENT_TYPE_VIDEO_WEBM :
            return "video/webm";
        case HTTP_CONTENT_TYPE_VIDEO_OGG :
            return "video/ogg";
        default :
            return NULL;
    }
}

HttpResponse *HttpRespondWithHtml(HttpResponse *response, HttpResponseCode status, Str *html) {
    if(!response || !html) {
        LOG_FATAL("invalid arguments.");
    }

    response->status_code  = status;
    response->content_type = HTTP_CONTENT_TYPE_TEXT_HTML;
    response->body         = StrDup(html);

    return response;
}

HttpResponse *HttpRespondWithFile(
    HttpResponse    *response,
    HttpResponseCode status,
    HttpContentType  content_type,
    const char      *filepath
) {
    if(!response || !filepath) {
        LOG_FATAL("invalid arguments.");
    }

    response->status_code  = status;
    response->content_type = content_type;

    if(!ReadCompleteFile(
           filepath,
           &response->body.data,
           &response->body.length,
           &response->body.capacity
       )) {
        LOG_ERROR("failed to read html file contents.");
        return NULL;
    }

    return response;
}


HttpResponse *HttpRespondTo(HttpResponse *response, int connfd) {
    if(!response || !connfd) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    const char *response_code = HttpResponseCodeToZstr(response->status_code);
    if(!response_code) {
        LOG_ERROR("invalid/unknown response code");
        return NULL;
    }

    const char *content_type = HttpContentTypeToZstr(response->content_type);
    if(!content_type) {
        LOG_ERROR("invalid/unknown content type");
        return NULL;
    }

    // http response
    Str rstr = StrInit();
    StrWriteFmt(
        &rstr,
        "HTTP/1.1 {}\r\n"
        "Server: beam/0.1\r\n"
        "Content-Type: {}\r\n"
        "Content-Length: {}\r\n",
        response_code,
        content_type,
        response->body.length
    );

    // http headers
    VecForeachPtr(&response->headers, header, {
        StrWriteFmt(&rstr, "{}: {}\r\n", header->key, header->value);
    });

    // response end, body start
    StrWriteFmt(&rstr, "\r\n");

    // response body
    StrReserve(&rstr, rstr.length + response->body.length);
    memcpy(StrEnd(&rstr), response->body.data, response->body.length);
    rstr.length += response->body.length;

    send(connfd, rstr.data, rstr.length, 0);

    StrDeinit(&rstr);

    return response;
}


void HttpResponseDeinit(HttpResponse *response) {
    if(!response) {
        LOG_FATAL("invalid arguments");
    }

    StrDeinit(&response->body);
    VecDeinit(&response->headers);
    response->content_type = HTTP_CONTENT_TYPE_INVALID;
    response->status_code  = HTTP_RESPONSE_CODE_INVALID;
}
