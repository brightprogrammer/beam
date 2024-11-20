/// file      : http.h
/// author    : Siddharth Mishra (admin@brightprogrammer.in)
/// copyright : Copyright (c) 2024, Siddharth Mishra, All rights reserved.
///
/// Provide HTTP constructs for beam.

#ifndef BEAM_HTTP_H
#define BEAM_HTTP_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

// beam
#include <beam/container/vec.h>

typedef enum {
    HTTP_REQUEST_METHOD_GET,
    HTTP_REQUEST_METHOD_POST,
    HTTP_REQUEST_METHOD_DELETE,
    HTTP_REQUEST_METHOD_PUT,
    HTTP_REQUEST_METHOD_PATCH,
    HTTP_REQUEST_METHOD_HEAD,
    HTTP_REQUEST_METHOD_OPTIONS,
    HTTP_REQUEST_METHOD_CONNECT,
    HTTP_REQUEST_METHOD_TRACE,
    HTTP_REQUEST_METHOD_UNKNOWN
} HttpRequestMethod;

///
/// Try to parse first few characters of given raw string
///
/// method[out]            : Parsed http method will be stored here.
/// raw_request_str[in]    : Raw request string recevied from client through a recv call.
/// remaining_size[in,out] : Provides an idea of remaining size of `raw_request_str` buffer,
///                          and returns the remaining size of buffer after parsing is complete.
///                          If parsing fails, this is left unchanged.
///
/// SUCCESS : Returns pointer after which parsing must continue.
///           remaining_size variable is updated with new remaining size.
/// FAILURE : Returns NULL. remaining_size variable is not altered.
///
const char *HttpRequestMethodParse(
    HttpRequestMethod *method,
    const char        *raw_request_str,
    size_t            *remaining_size
);

///
/// Parse and get HTTP url from raw request string.
///
/// url[out]               : Where pointer to url string will be stored. The created string
///                          is created using strdup() and hence must be destroyed after use.
/// raw_request_str[in]    : Position to start parsing from.
/// remaining_size[in,out] : Remaining size in raw_request_str.
///
/// SUCCESS : Position to continue parsing from
///           remaining_size variable is updated with new remaining size.
/// FAILURE : NULL. remaining_size variable is not altered.
///
const char *HttpUrlParse(const char **url, const char *raw_request_str, size_t *remaining_size);

///
/// Check whether the http request version matches that supported by this server.
///
/// NOTE: for now we recognize only HTTP/1.1
///
/// raw_request_str[in]    : Position to start parsing from.
/// remaining_size[in,out] : Remaining size in raw_request_str.
///
/// SUCCESS : Position to continue parsing from
///           remaining_size variable is updated with new remaining size.
/// FAILURE : NULL. remaining_size variable is not altered.
///
const char *HttpVersionValidate(const char *raw_request_str, size_t *remaining_size);

typedef struct {
    const char *key;
    const char *value;
} HttpHeader;

typedef Vec(HttpHeader) HttpHeaders;

///
/// Parse http header (key, value) pairs.
///
/// header[out]              : Where parsed data will be stored. (name, value) pair is created using
///                            strdup() and hence must be freed after use.
/// more_headers_remain[out] : If there are no more headers to parse, then this will be set to false.
/// raw_request_str[in]      : Position to start parsing from.
/// remaining_size[in,out]   : Remaining size in raw_request_str.
///
/// SUCCESS : Position to continue parsing from
///           remaining_size variable is updated with new remaining size.
/// FAILURE : NULL. remaining_size variable is not altered.
///
const char *HttpHeaderParse(
    HttpHeader *header,
    bool       *more_headers_remain,
    const char *raw_request_str,
    size_t     *remaining_size
);

///
/// Parse all HttpHeader structs into a HttpHeaders (Vec(HttpHeader)) struct.
///
/// headers[out]           : Parsed http headers will be stored in this vector.
/// raw_request_str[in]    : Position to start parsing http headers from.
/// remaining_size[in,out] : Hint about remaining size in raw_request_str and after parsing
///                          new remaining_size will be stored.
///
/// SUCCESS : Position to continue parsing from
/// FAILURE : NULL. remaining_size variable is not altered.
///
const char *
    HttpHeadersParseAll(HttpHeaders *headers, const char *raw_request_str, size_t *remaining_size);


///
/// Free all resources occupied by provided http headers vector.
///
/// headers[in,out] : Vector of http headers to be freed.
///
void HttpHeadersFreeAll(HttpHeaders *headers);

///
/// Find http header with given name.
///
/// headers[in] : Vector of headers to look in.
/// key[in]     : Header key to look for.
///
/// SUCCESS : A non-null value pointing to a header with matching name.
/// FAILURE : NULL
///
HttpHeader *HttpHeadersFind(HttpHeaders *headers, const char *key);

///
/// Contains parsed http request contents.
/// Call HttpRequestParse on this along with raw response string to fill
/// up this struct with parsed response.
///
typedef struct {
    HttpRequestMethod method;
    const char       *url;
    HttpHeaders       headers;

    const char *raw; ///< Raw request
    size_t      request_size;
} HttpRequest;

///
/// Parse http request (method, url, headers, body)
///
/// request[out]             : Where parsed data will be stored.
/// raw_request_str[in]      : Position to start parsing from.
/// remaining_size[in,out]   : Remaining size in raw_request_str.
///
/// SUCCESS : Position to continue parsing from
///           remaining_size variable is updated with new remaining size.
/// FAILURE : NULL. remaining_size variable is not altered.
///
const char *
    HttpRequestParse(HttpRequest *request, const char *raw_request_str, size_t *remaining_size);

///
/// Free all allocated resources created when parsing an Http request.
///
/// request[in,out] : Http request to be deinited.
///
void HttpRequestReset(HttpRequest *request);

typedef enum {
    // Informational responses
    HTTP_RESPONSE_CODE_CONTINUE            = 100,
    HTTP_RESPONSE_CODE_SWITCHING_PROTOCOLS = 101,
    HTTP_RESPONSE_CODE_PROCESSING          = 102,
    HTTP_RESPONSE_CODE_EARLY_HINTS         = 103,

    // Successful responses
    HTTP_RESPONSE_CODE_OK                            = 200,
    HTTP_RESPONSE_CODE_CREATED                       = 201,
    HTTP_RESPONSE_CODE_ACCEPTED                      = 202,
    HTTP_RESPONSE_CODE_NON_AUTHORITATIVE_INFORMATION = 203,
    HTTP_RESPONSE_CODE_NO_CONTENT                    = 204,
    HTTP_RESPONSE_CODE_RESET_CONTENT                 = 205,
    HTTP_RESPONSE_CODE_PARTIAL_CONTENT               = 206,
    HTTP_RESPONSE_CODE_MULTI_STATUS                  = 207,
    HTTP_RESPONSE_CODE_ALREADY_REPORTED              = 208,
    HTTP_RESPONSE_CODE_IM_USED                       = 226,

    // Redirection messages
    HTTP_RESPONSE_CODE_MULTIPLE_CHOICES   = 300,
    HTTP_RESPONSE_CODE_MOVED_PERMANENTLY  = 301,
    HTTP_RESPONSE_CODE_FOUND              = 302,
    HTTP_RESPONSE_CODE_SEE_OTHER          = 303,
    HTTP_RESPONSE_CODE_NOT_MODIFIED       = 304,
    HTTP_RESPONSE_CODE_USE_PROXY          = 305,
    HTTP_RESPONSE_CODE_TEMPORARY_REDIRECT = 307,
    HTTP_RESPONSE_CODE_PERMANENT_REDIRECT = 308,

    // Client error responses
    HTTP_RESPONSE_CODE_BAD_REQUEST                     = 400,
    HTTP_RESPONSE_CODE_UNAUTHORIZED                    = 401,
    HTTP_RESPONSE_CODE_PAYMENT_REQUIRED                = 402,
    HTTP_RESPONSE_CODE_FORBIDDEN                       = 403,
    HTTP_RESPONSE_CODE_NOT_FOUND                       = 404,
    HTTP_RESPONSE_CODE_METHOD_NOT_ALLOWED              = 405,
    HTTP_RESPONSE_CODE_NOT_ACCEPTABLE                  = 406,
    HTTP_RESPONSE_CODE_PROXY_AUTHENTICATION_REQUIRED   = 407,
    HTTP_RESPONSE_CODE_REQUEST_TIMEOUT                 = 408,
    HTTP_RESPONSE_CODE_CONFLICT                        = 409,
    HTTP_RESPONSE_CODE_GONE                            = 410,
    HTTP_RESPONSE_CODE_LENGTH_REQUIRED                 = 411,
    HTTP_RESPONSE_CODE_PRECONDITION_FAILED             = 412,
    HTTP_RESPONSE_CODE_PAYLOAD_TOO_LARGE               = 413,
    HTTP_RESPONSE_CODE_URI_TOO_LONG                    = 414,
    HTTP_RESPONSE_CODE_UNSUPPORTED_MEDIA_TYPE          = 415,
    HTTP_RESPONSE_CODE_RANGE_NOT_SATISFIABLE           = 416,
    HTTP_RESPONSE_CODE_EXPECTATION_FAILED              = 417,
    HTTP_RESPONSE_CODE_IM_A_TEAPOT                     = 418,
    HTTP_RESPONSE_CODE_MISDIRECTED_REQUEST             = 421,
    HTTP_RESPONSE_CODE_UNPROCESSABLE_ENTITY            = 422,
    HTTP_RESPONSE_CODE_LOCKED                          = 423,
    HTTP_RESPONSE_CODE_FAILED_DEPENDENCY               = 424,
    HTTP_RESPONSE_CODE_TOO_EARLY                       = 425,
    HTTP_RESPONSE_CODE_UPGRADE_REQUIRED                = 426,
    HTTP_RESPONSE_CODE_PRECONDITION_REQUIRED           = 428,
    HTTP_RESPONSE_CODE_TOO_MANY_REQUESTS               = 429,
    HTTP_RESPONSE_CODE_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
    HTTP_RESPONSE_CODE_UNAVAILABLE_FOR_LEGAL_REASONS   = 451,

    // Server error responses
    HTTP_RESPONSE_CODE_INTERNAL_SERVER_ERROR           = 500,
    HTTP_RESPONSE_CODE_NOT_IMPLEMENTED                 = 501,
    HTTP_RESPONSE_CODE_BAD_GATEWAY                     = 502,
    HTTP_RESPONSE_CODE_SERVICE_UNAVAILABLE             = 503,
    HTTP_RESPONSE_CODE_GATEWAY_TIMEOUT                 = 504,
    HTTP_RESPONSE_CODE_HTTP_VERSION_NOT_SUPPORTED      = 505,
    HTTP_RESPONSE_CODE_VARIANT_ALSO_NEGOTIATES         = 506,
    HTTP_RESPONSE_CODE_INSUFFICIENT_STORAGE            = 507,
    HTTP_RESPONSE_CODE_LOOP_DETECTED                   = 508,
    HTTP_RESPONSE_CODE_NOT_EXTENDED                    = 510,
    HTTP_RESPONSE_CODE_NETWORK_AUTHENTICATION_REQUIRED = 511
} HttpResponseCode;

///
/// Convert given HttpResponseCode to corresponding string.
///
/// code[in] : HttpResponseCode
///
/// SUCCESS : const char* - response code in string format
/// FAILURE : NULL (when code is not one of the known values)
///
const char *HttpResponseCodeToString(HttpResponseCode code);

typedef enum {
    HTTP_CONTENT_TYPE_TEXT_HTML,                         // text/html
    HTTP_CONTENT_TYPE_TEXT_PLAIN,                        // text/plain
    HTTP_CONTENT_TYPE_TEXT_CSS,                          // text/css
    HTTP_CONTENT_TYPE_TEXT_JAVASCRIPT,                   // text/javascript (deprecated)

    HTTP_CONTENT_TYPE_APPLICATION_JSON,                  // application/json
    HTTP_CONTENT_TYPE_APPLICATION_XML,                   // application/xml
    HTTP_CONTENT_TYPE_APPLICATION_JAVASCRIPT,            // application/javascript
    HTTP_CONTENT_TYPE_APPLICATION_PDF,                   // application/pdf
    HTTP_CONTENT_TYPE_APPLICATION_OCTET_STREAM,          // application/octet-stream
    HTTP_CONTENT_TYPE_APPLICATION_X_WWW_FORM_URLENCODED, // application/x-www-form-urlencoded
    HTTP_CONTENT_TYPE_APPLICATION_ZIP,                   // application/zip
    HTTP_CONTENT_TYPE_APPLICATION_MS_EXCEL,              // application/vnd.ms-excel
    HTTP_CONTENT_TYPE_APPLICATION_OPENXML_SPREADSHEET, // application/vnd.openxmlformats-officedocument.spreadsheetml.sheet

    HTTP_CONTENT_TYPE_IMAGE_JPEG,            // image/jpeg
    HTTP_CONTENT_TYPE_IMAGE_PNG,             // image/png
    HTTP_CONTENT_TYPE_IMAGE_GIF,             // image/gif
    HTTP_CONTENT_TYPE_IMAGE_BMP,             // image/bmp
    HTTP_CONTENT_TYPE_IMAGE_WEBP,            // image/webp
    HTTP_CONTENT_TYPE_IMAGE_SVG_XML,         // image/svg+xml

    HTTP_CONTENT_TYPE_AUDIO_MPEG,            // audio/mpeg
    HTTP_CONTENT_TYPE_AUDIO_OGG,             // audio/ogg
    HTTP_CONTENT_TYPE_AUDIO_WAV,             // audio/wav

    HTTP_CONTENT_TYPE_VIDEO_MP4,             // video/mp4
    HTTP_CONTENT_TYPE_VIDEO_OGG,             // video/ogg
    HTTP_CONTENT_TYPE_VIDEO_WEBM,            // video/webm

    HTTP_CONTENT_TYPE_MULTIPART_FORM_DATA,   // multipart/form-data
    HTTP_CONTENT_TYPE_MULTIPART_BYTERANGES,  // multipart/byteranges

    HTTP_CONTENT_TYPE_FONT_WOFF,             // font/woff
    HTTP_CONTENT_TYPE_FONT_WOFF2,            // font/woff2
    HTTP_CONTENT_TYPE_APPLICATION_FONT_WOFF, // application/font-woff (older syntax)

    HTTP_CONTENT_TYPE_APPLICATION_LD_JSON,   // application/ld+json
    HTTP_CONTENT_TYPE_APPLICATION_GRAPHQL,   // application/graphql
    HTTP_CONTENT_TYPE_TEXT_CSV               // text/csv
} HttpContentType;

///
/// Convert given HttpContentType to corresponding string.
///
/// code[in] : HttpContentType
///
/// SUCCESS : const char* - content-type in string format
/// FAILURE : NULL (when content-type is not one of the known values)
///
const char *HttpContentTypeToString(HttpContentType content_type);

typedef struct {
    HttpContentType  content_type;
    HttpResponseCode status_code;
    HttpHeaders      headers;
    void            *body; // response body, like an html data
    size_t           body_size;
    size_t           body_capacity;
} HttpResponse;

///
/// Initialize given http response as html response.
/// This is best to use when serving a web-page or when serving a static
/// webpage stored in a file.
///
/// response[out]    : Where inited http response will be stored.
/// status_code[in]  : Http status code to initialize this for.
/// content_type[in] : Type of content the file contains. This will be used to
///                    automatically add a valid header in http response.
/// filename[in]     : Name/path of html file to be loaded.
///
/// SUCCESS : `response`
/// FAILURE : NULL
///
HttpResponse *HttpResponseInitFromFile(
    HttpResponse    *response,
    HttpResponseCode status_code,
    HttpContentType  content_type,
    const char      *filename
);


///
/// Send prepared http response.
///
/// response[in] : Prepared response to be sent.
/// connfd[in]   : Socket file descriptor to called send upon
///
/// SUCCESS : `response`
/// FAILURE : NULL
///
HttpResponse *HttpResponseSend(HttpResponse *response, int connfd);

///
/// Free all resources occupued by provided http response.
///
/// response[in,out] : HttpResponse object to be resetted.
///
void HttpResponseReset(HttpResponse *response);

#endif // BEAM_HTTP_H
