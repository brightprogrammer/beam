// sockets
#include <unistd.h>
#include <arpa/inet.h>

#include <Misra.h>
#include <Beam/Http.h>

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
    Str response = StrInit();

    StrWriteFmt(
        &response,

        // http response
        "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html; "
        "charset=UTF-8\r\nContent-Length: 12\r\n\r\n"

        // html body
        "<html><head><title>500</title></head><body>{}</body></html>",

        // body params
        msg ? msg : "internal server error, beam is sorry :-("
    );

    send(connfd, response.data, response.length, 0);

    StrDeinit(&response);
}

///
/// Send a html response
///
/// Function guarantees that an http response gets sent if atleast connfd is correct.
/// Even if html file fails to load, or any other error occurs, an internal server
/// error response gets sent automatically.
///
/// html[in]   : Str object containing html response.
/// code[in]   : HTTP response code.
/// connfd[in] : Connection socket file descriptor.
///
void RespondWithHtml(Str *html, HttpResponseCode code, int connfd) {
    if(!html || connfd < 0) {
        LOG_ERROR("Invalid html or connection");
        SendInternalServerErrorResponse(NULL, connfd);
        return;
    }

    HttpResponse response = HttpResponseInit();
    HttpRespondWithHtml(&response, code, html);
    HttpRespondTo(&response, connfd);
    HttpResponseDeinit(&response);
}

void ServerMain(int connfd, HttpRequest *request) {
    Str html = StrInitFromZstr("hello");
    RespondWithHtml(&html, HTTP_RESPONSE_CODE_OK, connfd);
    StrDeinit(&html);
}

int main() {
    LogInit(true);

    // create main socket that the server listens on
    i32 sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if(-1 == sockfd) {
        LOG_SYS_FATAL("socket() failed");
    }

    // allow reusing of socket
    i32 res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&LVAL(1L), sizeof(int));
    if(-1 == res) {
        close(sockfd);
        LOG_SYS_FATAL("setsockopt() failed");
    }

    // bind socket to an addres
    struct sockaddr_in6 server_addr = {0};
    server_addr.sin6_family         = AF_INET6;
    server_addr.sin6_addr           = in6addr_any;
    server_addr.sin6_port           = htons(PORT);
    res = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(-1 == res) {
        close(sockfd);
        LOG_SYS_ERROR("bind() failed");
    }

    // listen for incoming connections on the socket
    res = listen(sockfd, 10);
    if(-1 == res) {
        close(sockfd);
        LOG_SYS_FATAL("listen() failed ");
    }
    WriteFmtLn("Listening on port {}...\n", PORT);

    Str buf = StrInit();
    StrReserve(&buf, UINT16_MAX - 1);

    HttpRequest req = HttpRequestInit();
    while(true) {
        struct sockaddr_storage client_addr = {0};
        socklen_t               addrlen     = sizeof(client_addr);
        int                     connfd = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen);
        if(-1 == connfd) {
            close(sockfd);
            LOG_SYS_ERROR("listen() failed");
        }

        i64 recv_size = recv(connfd, buf.data, buf.capacity, 0);
        if(-1 == recv_size) {
            close(connfd);
            LOG_SYS_ERROR("recv() failed");
            continue;
        }
        buf.length = (u64)recv_size;

        LOG_INFO("REQUEST : \n{}", buf);

        if(!HttpRequestParse(&req, buf.data)) {
            LOG_ERROR("failed to parse http request");
            LOG_ERROR("request was : \n{}", buf);
            continue;
        }

        ServerMain(connfd, &req);

        HttpRequestDeinit(&req);

        close(connfd);
    }

    close(sockfd);

    return EXIT_SUCCESS;
}
