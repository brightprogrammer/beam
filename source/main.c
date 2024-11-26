#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

// sockets
#include <unistd.h>
#include <arpa/inet.h>

// beam
#include <beam/log.h>
#include <beam/file.h>
#include <beam/http.h>
#include <beam/container/vec.h>
#include <beam/container/list.h>
#include <beam/container/string.h>
#include <frontend/frontend.h>

#define PORT 3000

// code.brightprogrammer.in
// serve directory

///
/// I guarantee that I'll send a response if atleast I get a valid connfd.
///
/// msg[in]    : A custom message to be sent.
/// connfd[in] : Connection socket file descriptor.
///
void SendInternalServerErrorResponse(const char *msg, int connfd) {
    static char *html_start = "<html><head><title>500</title></head><body>";
    msg                     = msg ? msg : "internal server error, beam is sorry :-(";
    static char *html_end   = "</body></html>";

    static const char *response_header =
        "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html; "
        "charset=UTF-8\r\nContent-Length: 12\r\n\r\n";

    size_t len = snprintf(NULL, 0, "%s%s%s%s", response_header, html_start, msg, html_end) + 1;
    char  *complete_response = malloc(len);
    if(!complete_response) {
        LOG_ERROR("malloc() failed : %s", strerror(errno));
        send(connfd, msg, strlen(msg), 0);
        return;
    }

    snprintf(complete_response, len, "%s%s%s%s", response_header, html_start, msg, html_end);
    send(connfd, complete_response, len, 0);
    free(complete_response);
}

///
/// Send a html response
///
/// Function guarantees that an http response gets sent if atleast connfd is correct.
/// Even if html file fails to load, or any other error occurs, an internal server
/// error response gets sent automatically.
///
/// path[in]   : Path of html file to be served.
/// code[in]   : HTTP response code.
/// connfd[in] : Connection socket file descriptor.
///
void SendHtmlResponse(Html *html, HttpResponseCode code, int connfd) {
    if(!html || connfd < 0) {
        LOG_ERROR("invalid path");
        SendInternalServerErrorResponse(NULL, connfd);
        return;
    }

    HttpResponse response = {0};
    HttpResponseInitForHtml(&response, code, html);
    HttpResponseSend(&response, connfd);
    HttpResponseReset(&response);
}

///
/// Serve all files at given path.
///
/// connfd[in]  : Connection socket file descriptor.
/// path[in]    : Path to be served.
/// request[in] : HTTP equest received.
///
void ServeDirectory(Html *html, const char *local_path, HttpRequest *request) {
    if(!html || !local_path || !request) {
        LOG_ERROR("invalid arguments.");
        return;
    }

    DirContents dir = {0};
    if(ReadDirContents(&dir, local_path)) {
        WrapDirContents(html, &dir);
        VecDeinit(&dir);
    } else {
        size_t file_path_len = strlen(local_path);

        // remove extra "/" if present
        if(local_path[file_path_len - 1] == '/') {
            file_path_len--;
            ((char *)local_path)[file_path_len] = 0;
        }

        // if file has some size, serve it
        size_t file_size = GetFileSize(local_path);
        if(file_size) {
            WrapFileContent(html, local_path);
        } else {
            LOG_ERROR("failed to get file/directory.");
            Wrap404(html);
            return;
        }
    }
}

void ServerMain(int connfd, HttpRequest *request) {
    HttpHeader *host = HttpHeadersFind(&request->headers, "Host");
    if(!host) {
        LOG_ERROR("host not specified.");
        return;
    }

    Html html = {0};
    ListInit(&html, StringInitCopy, StringDeinitCopy);

    if(0 == strcmp(host->value, "code.brightprogrammer.in") ||
       0 == strncmp(host->value, "code.localhost", strlen("code.localhost"))) {
        String path = {0};
#if __linux__
        StringInitFmt(&path, "%s/blog/%s", getenv("HOME"), request->url);
#elif __APPLE__
        StringInitFmt(&path, "/Users/misra/Desktop/blog/%s", request->url);
#endif
        ServeDirectory(&html, path.data, request);
        StringDeinit(&path);
    } else {
        if(0 == strcmp(request->url, "/")) {
            HtmlAppendFmt(
                &html,
                "This is a self-hosted website. Beam now hosts it's own code.<br />"
                "Wanna check it out?</br>"
                "Visit <a href=\"%s://code.%s/\" target=\"_blank\">code.brightprogrammer.in</a>",
                strstr(host->value, "brightprogrammer") ? "https" : "http",
                host->value
            );
        } else {
            Wrap404(&html);
        }
    }

    WrapBase(WrapContent(&html));
    SendHtmlResponse(&html, HTTP_RESPONSE_CODE_OK, connfd);
    ListDeinit(&html);
}

int main() {
    // create main socket that the server listens on
    int sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if(-1 == sockfd) {
        LOG_ERROR("socket() : %s", strerror(errno));
        return EXIT_FAILURE;
    }

    // allow reusing of socket
    int res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, ((int[]) {1}), sizeof(int));
    if(-1 == res) {
        LOG_ERROR("setsockopt() : %s", strerror(errno));
        close(sockfd);
        return EXIT_FAILURE;
    }

    // bind socket to an addres
    struct sockaddr_in6 server_addr = {0};
    server_addr.sin6_family         = AF_INET6;
    server_addr.sin6_addr           = in6addr_any;
    server_addr.sin6_port           = htons(PORT);
    res = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(-1 == res) {
        LOG_ERROR("bind() failed : %s", strerror(errno));
        close(sockfd);
        return EXIT_FAILURE;
    }

    // listen for incoming connections on the socket
    res = listen(sockfd, 10);
    if(-1 == res) {
        LOG_ERROR("listen() failed : %s", strerror(errno));
        close(sockfd);
        return EXIT_FAILURE;
    }
    printf("listening on port %d...\n", PORT);

    char   buf[UINT16_MAX] = {0};
    size_t buf_size        = UINT16_MAX - 1;

    HttpRequest req = {0};
    while(true) {
        struct sockaddr_storage client_addr = {0};
        socklen_t               addrlen     = sizeof(client_addr);
        int                     connfd = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen);
        if(-1 == connfd) {
            LOG_ERROR("listen() failed : %s", strerror(errno));
            close(sockfd);
            return EXIT_FAILURE;
        }

        ssize_t recv_size = recv(connfd, buf, buf_size, 0);
        if(-1 == recv_size) {
            LOG_ERROR("recv() failed : %s", strerror(errno));
            close(connfd);
            continue;
        }

        LOG_INFO("REQUEST :\%s", buf);

        size_t rem_size = recv_size;
        if(!HttpRequestParse(&req, buf, &rem_size)) {
            LOG_ERROR("failed to parse http request");
            LOG_ERROR("request was :\n%s", buf);
            continue;
        }

        ServerMain(connfd, &req);

        HttpRequestReset(&req);

        close(connfd);
    }

    close(sockfd);

    return EXIT_SUCCESS;
}
