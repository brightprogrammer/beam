/// file      : http.c
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Provide HTTP constructs for beam.

// socket
#include <arpa/inet.h>

// beam
#include <beam/http.h>
#include <beam/log.h>

// stat
#include <sys/stat.h>

///
/// Get size of file without opening it.
///
/// filename[in] : Name/path of file.
///
/// SUCCESS : Non-negative value representing size of file in bytes.
/// FAILURE : -1
///
static int64_t GetFileSize(const char *filename);

///
/// Read complete contents of file at once.
///
/// Pointer returned is malloc'd and hence must be freed after use.
/// The returned pointer can also be reused by providing pointer to it
/// in `data` parameter.
///
/// `realloc` is called on `*data` in order to expand it's size.
/// If `*capacity` exceeds the size of file to be loaded, then no reallocation
/// is performed. This means the provided buffer will automatically be expanded
/// if required.
///
/// The returned buffer is null-terminated just-in-case.
///
/// filename[in]     : Name/path of file to be read.
/// data[in,out]     : Memory buffer where loaded file will be stored.
/// file_size[out]   : Complete size of file in bytes will be stored here.
/// capacity[in,out] : Hints towards current capacity of `data` buffer.
///                    New capacity of `data` buffer is automatically stored here if
///                    realloc is performed.
///
/// SUCCESS : Returns a malloc'd array with read file contents.
/// FAILURE : NULL
///
static void *
    ReadCompleteFile(const char *filename, void **data, size_t *file_size, size_t *capacity);

const char *HttpRequestMethodParse(
    HttpRequestMethod *method,
    const char        *raw_request_str,
    size_t            *remaining_size
) {
    if(!method || !raw_request_str || !remaining_size) {
        LOG_ERROR("invalid arguments");
        return NULL;
    }

    size_t rem_size = *remaining_size;
    if(!rem_size) {
        LOG_ERROR("unsufficient size to parse http url");
        return NULL;
    }

    const char *line_end = memchr(raw_request_str, '\r', rem_size);
    if(!line_end || '\n' != line_end[1]) {
        LOG_ERROR("failed to get first line of http request.");
    }

    // method name and request url are separated by a space
    const char *method_end = memchr(raw_request_str, ' ', rem_size);
    if(!method_end) {
        LOG_ERROR("invalid http request method.");
        return NULL;
    }
    if(method_end >= line_end) {
        LOG_ERROR("malformed http request, method end exceeds line end");
        return NULL;
    }

    // compute method size
    size_t method_size = method_end - raw_request_str;
    if(!method_size) {
        LOG_ERROR("invalid http request method.");
        return NULL;
    }

    // compare based on method_size for faster search
    if(method_size == 7) {
        if(0 == strncmp(raw_request_str, "OPTIONS", 7)) {
            raw_request_str += 8;
            rem_size        -= 8;
            *method          = HTTP_REQUEST_METHOD_OPTIONS;
        } else if(0 == strncmp(raw_request_str, "CONNECT", 7)) {
            raw_request_str += 8;
            rem_size        -= 8;
            *method          = HTTP_REQUEST_METHOD_CONNECT;
        } else {
            *method = HTTP_REQUEST_METHOD_UNKNOWN;
        }
    } else if(method_size == 6) {
        if(0 == strncmp(raw_request_str, "DELETE", 6)) {
            raw_request_str += 7;
            rem_size        -= 7;
            *method          = HTTP_REQUEST_METHOD_DELETE;
        } else {
            *method = HTTP_REQUEST_METHOD_UNKNOWN;
        }
    } else if(method_size == 5) {
        if(0 == strncmp(raw_request_str, "PATCH", 5)) {
            raw_request_str += 7;
            rem_size        -= 7;
            *method          = HTTP_REQUEST_METHOD_PATCH;
        } else if(0 == strncmp(raw_request_str, "TRACE", 5)) {
            raw_request_str += 7;
            rem_size        -= 7;
            *method          = HTTP_REQUEST_METHOD_TRACE;
        } else {
            *method = HTTP_REQUEST_METHOD_UNKNOWN;
        }
    } else if(method_size == 4) {
        if(0 == strncmp(raw_request_str, "POST", 4)) {
            raw_request_str += 5;
            rem_size        -= 5;
            *method          = HTTP_REQUEST_METHOD_POST;
        } else {
            *method = HTTP_REQUEST_METHOD_UNKNOWN;
        }
    } else if(method_size == 3) {
        if(0 == strncmp(raw_request_str, "GET", 3)) {
            raw_request_str += 4;
            rem_size        -= 4;
            *method          = HTTP_REQUEST_METHOD_GET;
        } else if(0 == strncmp(raw_request_str, "PUT", 3)) {
            raw_request_str += 4;
            rem_size        -= 4;
            *method          = HTTP_REQUEST_METHOD_PUT;
        } else {
            *method = HTTP_REQUEST_METHOD_UNKNOWN;
        }
    } else {
        *method = HTTP_REQUEST_METHOD_UNKNOWN;
    }

    *remaining_size = rem_size;
    return raw_request_str;
}


const char *HttpUrlParse(const char **url, const char *raw_request_str, size_t *remaining_size) {
    if(!url || !raw_request_str || !remaining_size) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    size_t rem_size = *remaining_size;
    if(!rem_size) {
        LOG_ERROR("unsufficient size to parse http url");
        return NULL;
    }

    const char *line_end = memchr(raw_request_str, '\r', rem_size);
    if(!line_end || '\n' != line_end[1]) {
        LOG_ERROR("failed to get first line of http request.");
    }

    // parse request url
    const char *url_end = memchr(raw_request_str, ' ', rem_size);
    if(!url_end) {
        LOG_ERROR("failed to get request url end.");
        return NULL;
    }
    if(url_end >= line_end) {
        LOG_ERROR("malformed http request, url end exceeds line end");
        return NULL;
    }

    size_t url_len   = url_end - raw_request_str;
    *url             = strndup(raw_request_str, url_len);
    raw_request_str += url_len + 1;
    rem_size        -= url_len + 1;

    *remaining_size = rem_size;
    return raw_request_str;
}


const char *HttpVersionValidate(const char *raw_request_str, size_t *remaining_size) {
    if(!raw_request_str || !remaining_size) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    // parse http request version
    if(10 > *remaining_size) {
        LOG_ERROR("insufficient data to parse http version.");
        return NULL;
    }

    // compare version with http/1.1
    if(0 == strncmp(raw_request_str, "HTTP/1.1\r\n", 10)) {
        *remaining_size -= 10;
        return raw_request_str + 10;
    } else {
        LOG_ERROR("unknown/unsupported http version.");
        return NULL;
    }
}


const char *HttpHeaderParse(
    HttpHeader *header,
    bool       *more_headers_remain,
    const char *raw_request_str,
    size_t     *remaining_size
) {
    if(!header || !raw_request_str || !more_headers_remain || !remaining_size) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    size_t rem_size = *remaining_size;
    if(!rem_size) {
        return raw_request_str;
    }

    // total length of header
    const char *header_end = memchr(raw_request_str, '\r', rem_size);
    if(!header_end) {
        LOG_ERROR("failed to find header end.");
        return NULL;
    }
    if('\n' != header_end[1]) {
        LOG_ERROR("invalid header end, CRLF expected.");
        return NULL;
    }
    size_t header_size = header_end - raw_request_str;
    if(3 > header_size) {
        LOG_ERROR("insufficient size to parse header");
        return NULL;
    }

    // get key
    const char *key     = raw_request_str;
    const char *key_end = memchr(key, ':', rem_size);
    if(!key_end) {
        LOG_ERROR("failed to find header key end.");
        return NULL;
    }
    if(key_end > header_end) {
        LOG_ERROR("malformed http request, header key end exceeds header end");
        return NULL;
    }
    if(' ' != key_end[1]) {
        LOG_ERROR("expacted <space> after key end.");
        return NULL;
    }
    if(key_end == key) {
        LOG_ERROR("invalid header key.");
        return NULL;
    }

    rem_size        -= key_end - key + 2; // adjusting for ":" and " "
    raw_request_str += key_end - key + 2;

    // get value
    const char *value     = key_end + 2; // adjusting for ":" and " "
    const char *value_end = memchr(value, '\r', rem_size);
    if(!value_end) {
        LOG_ERROR("failed to find header value end.");
        return NULL;
    }
    if('\n' != value_end[1]) {
        LOG_ERROR("expected CRLF at end of header value.");
        return NULL;
    }
    if(value_end == value) {
        LOG_ERROR("invalid header value.");
        return NULL;
    }

    rem_size        -= value_end - value + 2; // adjusting for CRLF
    raw_request_str += value_end - value + 2;

    // create header key-value pair clone
    header->key = strndup(key, key_end - key);
    if(!header->key) {
        LOG_ERROR("strndup() failed : %s.", strerror(errno));
        return NULL;
    }
    header->value = strndup(value, value_end - value);
    if(!header->value) {
        LOG_ERROR("strndup() failed : %s.", strerror(errno));
        return NULL;
    }

    // if the request string now contains CRLF, this means
    // there are no headers left or parse
    // this means we've parsed all possible headers
    if('\r' == raw_request_str[0] && '\n' == raw_request_str[1]) {
        *more_headers_remain  = false;
        rem_size             -= 2;
        raw_request_str      += 2;
    }

    *remaining_size = rem_size;
    return raw_request_str;
}


const char *
    HttpHeadersParseAll(HttpHeaders *headers, const char *raw_request_str, size_t *remaining_size) {
    if(!headers || !raw_request_str || !remaining_size) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    // temporary variable to avoid directly changing remainning size
    size_t rem_size = *remaining_size;
    if(!rem_size) {
        return raw_request_str;
    }

    VecInit(headers, NULL, NULL);

    // parse all headers
    HttpHeader header;
    bool       more_headers_remain = true;
    while(more_headers_remain) {
        raw_request_str =
            HttpHeaderParse(&header, &more_headers_remain, raw_request_str, &rem_size);
        if(!raw_request_str) {
            LOG_ERROR("failed to parse http header.");
            return NULL;
        }

        if(!VecPush(headers, &header)) {
            LOG_ERROR("failed to insert header into vector.");
        }
    }

    *remaining_size = rem_size;
    return raw_request_str;
}


void HttpHeadersFreeAll(HttpHeaders *headers) {
    if(!headers) {
        LOG_ERROR("invalid arguments.");
        return;
    }

    HttpHeader *header;
    size_t      iter;
    VecForeachPtr(headers, header, iter) {
        if(header->key) {
            free((void *)header->key);
        }

        if(header->value) {
            free((void *)header->value);
        }
    }

    VecDeinit(headers);
}


HttpHeader *HttpHeadersFind(HttpHeaders *headers, const char *key) {
    if(!headers || !key) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    HttpHeader *header;
    size_t      iter;
    VecForeachPtr(headers, header, iter) {
        if(0 == strcmp(header->key, key)) {
            return header;
        }
    }

    return NULL;
}


const char *
    HttpRequestParse(HttpRequest *request, const char *raw_request_str, size_t *remaining_size) {
    if(!request || !raw_request_str || !remaining_size) {
        LOG_ERROR("invalid arguments");
        return NULL;
    }

    // temporary variable to avoid directly changing remainning size
    size_t rem_size = *remaining_size;
    if(!rem_size) {
        return raw_request_str;
    }

    raw_request_str = HttpRequestMethodParse(&request->method, raw_request_str, &rem_size);
    if(!raw_request_str) {
        LOG_ERROR("failed to parse http request method.");
        return NULL;
    }

    raw_request_str = HttpUrlParse(&request->url, raw_request_str, &rem_size);
    if(!raw_request_str) {
        LOG_ERROR("failed to parse http url.");
        return NULL;
    }

    raw_request_str = HttpVersionValidate(raw_request_str, &rem_size);
    if(!raw_request_str) {
        LOG_ERROR("invalid/unsupported http version.");
        return NULL;
    }

    raw_request_str = HttpHeadersParseAll(&request->headers, raw_request_str, &rem_size);
    if(!raw_request_str) {
        LOG_ERROR("failed to parse all http headers.");
        return NULL;
    }

    // set request size before altering remaining_size
    request->request_size = *remaining_size;
    *remaining_size       = rem_size;
    return raw_request_str;
}


void HttpRequestReset(HttpRequest *request) {
    if(!request) {
        LOG_ERROR("invalid arguments");
        return;
    }

    if(request->url) {
        free((void *)request->url);
    }

    HttpHeadersFreeAll(&request->headers);

    memset(request, 0, sizeof(HttpRequest));
}


const char *HttpResponseCodeToString(HttpResponseCode code) {
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


const char *HttpContentTypeToString(HttpContentType type) {
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


HttpResponse *HttpResponseInitFromFile(
    HttpResponse    *response,
    HttpResponseCode status_code,
    HttpContentType  content_type,
    const char      *filename
) {
    if(!response || !filename) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    response->status_code  = status_code;
    response->content_type = content_type;

    const char *html =
        ReadCompleteFile(filename, &response->body, &response->body_size, &response->body_capacity);
    if(!html) {
        LOG_ERROR("failed to read html file contents.");
        return NULL;
    }

    response->body = (void *)html;

    return response;
}


HttpResponse *HttpResponseSend(HttpResponse *response, int connfd) {
    if(!response || !connfd) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    const char *response_code = HttpResponseCodeToString(response->status_code);
    if(!response_code) {
        LOG_ERROR("invalid/unknown response code");
        return NULL;
    }

    const char *content_type = HttpContentTypeToString(response->content_type);
    if(!content_type) {
        LOG_ERROR("invalid/unknown content type");
        return NULL;
    }

    // TODO: use headers here
    // reuse allocated buffer here??

    int http_response_size = snprintf(
        NULL,
        0,
        "HTTP/1.1 %s\r\n"
        "Server: beam/0.1\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "\r\n",
        response_code,
        content_type,
        response->body_size
    );
    if(0 >= http_response_size) {
        LOG_ERROR("snprintf() failed : %s.", strerror(errno));
        return NULL;
    }

    size_t total_send_size = http_response_size + response->body_size;
    char  *send_data       = malloc(total_send_size + 1);

    snprintf(
        send_data,
        total_send_size,
        "HTTP/1.1 %s\r\n"
        "Server: beam/0.1\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "\r\n",
        response_code,
        content_type,
        response->body_size
    );

    memcpy(send_data + http_response_size, response->body, response->body_size);

    send(connfd, send_data, total_send_size, 0);

    free(send_data);

    return response;
}


void HttpResponseReset(HttpResponse *response) {
    if(!response) {
        LOG_ERROR("invalid arguments");
        return;
    }

    if(response->body) {
        free(response->body);
    }

    HttpHeadersFreeAll(&response->headers);

    memset(response, 0, sizeof(HttpResponse));
}


static int64_t GetFileSize(const char *filename) {
    if(!filename) {
        LOG_ERROR("invalid arguments.");
        return -1;
    }

    struct stat file_stat;
    if(0 == stat(filename, &file_stat)) {
        return file_stat.st_size;
    } else {
        LOG_ERROR("failed to get file size : %s.", strerror(errno));
        return -1;
    }
}


static void *
    ReadCompleteFile(const char *filename, void **data, size_t *file_size, size_t *capacity) {
    if(!filename || !data || !file_size || !capacity) {
        LOG_ERROR("invalid arguments.");
        return NULL;
    }

    // get actual size of file
    int64_t size = GetFileSize(filename);
    if(-1 == size) {
        LOG_ERROR("failed to get file size");
        return NULL;
    }

    // allocate memory to hold the file contents if required
    void *buffer = *data;
    if(*capacity < (size_t)size) {
        buffer = realloc(buffer, size + 1);
        if(!buffer) {
            LOG_ERROR("malloc() failed : %s.", strerror(errno));
            return NULL;
        }

        *capacity = size + 1;
    }

    // Open the file in binary mode
    FILE *file = fopen(filename, "rb");
    if(!file) {
        LOG_ERROR("fopen() failed : %s.", strerror(errno));
        free(buffer);
        return NULL;
    }

    // Read the entire file into the buffer
    fread(buffer, 1, size, file);

    // Close the file and return the buffer
    fclose(file);

    ((char *)buffer)[size] = 0; // null-termination for just in case.
    *data                  = buffer;
    *file_size             = size;
    return buffer;
}
