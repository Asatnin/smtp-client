//
// Created on 1/3/18.
//

#include "smtp.h"

/*
 * return socket file descriptor after establishing connection to smtp server
 */
int connectToServer(const char *serverUrl, const unsigned short serverPort) {
    int socketFd = -1;
    struct sockaddr_in serverAddress = {0};
    struct hostent *host = NULL;

    if ((host = gethostbyname(serverUrl)) == NULL) {
        perror("error while calling gethostbyname");
        exit(-1);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    serverAddress.sin_addr = *((struct in_addr *)host->h_addr);

    socketFd = socket(PF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        perror("socket creation error...\n");
        exit(-1);
    }

    if (connect(socketFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        close(socketFd);
        perror("connection error...\n");
        exit(-1);
    }

    return socketFd;
}


void sendNoSsl(int server,
              char *from,
              char *to,
              char *replyto,
              char *subject,
              char *msg,
              char *msghtml) {
    char buf[BUF_SIZE];
    int bytes;

    // mail from: string
    char mailFrom[STR_LEN] = "MAIL FROM: <";
    strcat(mailFrom, from);
    strcat(mailFrom, ">\r\n");
    send(server, mailFrom, strlen(mailFrom), 0);
    printf("Client: %s", mailFrom);

    // server respond to mail from
    bytes = read(server, buf, sizeof(buf));
    buf[bytes] = '\0';
    printf("Server: %s", buf);

    // rcp to: string
    char rcptTo[STR_LEN] = "RCPT TO: <";
    strcat(rcptTo, to);
    strcat(rcptTo, ">\r\n");
    send(server, rcptTo, strlen(rcptTo), 0);
    printf("Client: %s", rcptTo);

    // server respond to rcptTo
    bytes = read(server, buf, sizeof(buf));
    buf[bytes] = '\0';
    printf("Server: %s", buf);

    // DATA\r\n
    char *data= "DATA\r\n";
    send(server, data, strlen(data), 0);
    printf("Client: %s", data);

    // server respond to DATA
    bytes = read(server, buf, sizeof(buf));
    buf[bytes] = '\0';
    printf("Server: %s", buf);

    // send all data to server

    //send from
    char tmpData[STR_LEN] = "From: ";
    strcat(tmpData, from);
    strcat(tmpData, "\r\n");
    send(server, tmpData, strlen(tmpData), 0);
    printf("Client: %s", tmpData);

    // send to
    memset(tmpData, 0, sizeof(tmpData));
    memcpy(tmpData, "To: ", 4);
    strcat(tmpData, to);
    strcat(tmpData, "\r\n");
    send(server, tmpData, strlen(tmpData), 0);
    printf("Client: %s", tmpData);

    // subject
    memset(tmpData, 0, sizeof(tmpData));
    memcpy(tmpData, "Subject: ", 9);
    strcat(tmpData, subject);
    strcat(tmpData, "\r\n");
    send(server, tmpData, strlen(tmpData), 0);
    printf("Client: %s", tmpData);

    // message text
    memset(tmpData, 0, sizeof(tmpData));
    memcpy(tmpData, "\r\n", 2);
    strcat(tmpData, msg);
    strcat(tmpData, "\r\n.\r\n");
    send(server, tmpData, strlen(tmpData), 0);
    printf("Client: %s", tmpData);

    // server respond to DATA
    bytes = read(server, buf, sizeof(buf));
    buf[bytes] = '\0';
    printf("Server: %s", buf);


    close(server);
}

int sendMail(char *hostname,
             const unsigned short portnum,
             char *from,
             char *to,
             char *replyto,
             char *subject,
             char *msg,
             char *msghtml) {
    SSL_CTX *ctx;
    int server;
    SSL *ssl;
    char buf[BUF_SIZE];
    int bytes;

    server = connectToServer(hostname, portnum);
    if(server <= 0){
        perror("connect to server failed");
        return -1;
    }

    // Starttls first STARTTLS
    char buffer[BUF_SIZE] = {0};
    char buffer1[BUF_SIZE] = {0};

    char *hello = "EHLO gmail.com\r\n";
    char *helloTls = "STARTTLS\r\n";

    // First read after establishing connection
    int valread = read(server, buffer, BUF_SIZE);
    printf("Server: %s", buffer);

    memset(buffer, 0, sizeof buffer);
    buffer[0] = '\0';

    // Send EHLO hostname to server
    send(server, hello, strlen(hello), 0);
    printf("Client: %s", hello);

    valread = read(server, buffer, BUF_SIZE);
    printf("Server: %s", buffer);

    memset(buffer, 0, sizeof buffer);
    buffer[0] = '\0';

    sendNoSsl(server, from, to, replyto, subject, msg, msghtml);

    return 0;

    // try to starttls
    send(server, helloTls, strlen(helloTls), 0);
    printf("Client: %s", helloTls);

    valread = read(server, buffer1, BUF_SIZE);
    printf("Server: %s", buffer1);

    // Prepare ssl routines
    ctx = initSSLContext();
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, server);

    // check for ssl errors. exit when there are
    sslError(ssl, 1);

    // start ssl connection
    if (SSL_connect(ssl) == -1) {
        perror("SSL_connect() failed");
        exit(-1);
    }

    // now ssl ehlo
    SSL_write(ssl, hello, strlen(hello));
    printf("Client: %s", hello);

    // server ssl read to ehlo
    bytes = SSL_read(ssl, buf, sizeof(buf));
    buf[bytes] = '\0';
    printf("Server: %s", buf);

    // mail from: string
    char mailFrom[STR_LEN] = "MAIL FROM: <";
    strcat(mailFrom, from);
    strcat(mailFrom, ">\r\n");
    SSL_write(ssl, mailFrom, strlen(mailFrom));
    printf("Client: %s", mailFrom);

    // server respond to mail from
    bytes = SSL_read(ssl, buf, sizeof(buf));
    buf[bytes] = '\0';
    printf("Server: %s", buf);

    // rcp to: string
    char rcptTo[STR_LEN] = "RCPT TO: <";
    strcat(rcptTo, to);
    strcat(rcptTo, ">\r\n");
    SSL_write(ssl, rcptTo, strlen(rcptTo));
    printf("Client: %s", rcptTo);

    // server respond to rcptTo
    bytes = SSL_read(ssl, buf, sizeof(buf));
    buf[bytes] = '\0';
    printf("Server: %s", buf);

    // DATA\r\n
    char *data= "DATA\r\n";
    SSL_write(ssl, data, strlen(data));
    printf("Client: %s", data);

    // server respond to DATA
    bytes = SSL_read(ssl, buf, sizeof(buf));
    buf[bytes] = '\0';
    printf("Server: %s", buf);

    close(server);

    return 0;
}

SSL_CTX* initSSLContext() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    method = SSLv23_client_method();
    ctx = SSL_CTX_new(method);
    if (ctx == NULL) {
        ERR_print_errors_fp(stderr);
        exit(-1);
    }
    return ctx;
}

void sslError(SSL *ssl, int received) {
    const int err = SSL_get_error(ssl, received);

    if (err == SSL_ERROR_NONE) {
        printf("SSL OK\n");
        return;
    } else if (err == SSL_ERROR_WANT_READ) {
        perror("SSL_ERROR_WANT_READ");
        SSL_shutdown(ssl);
        kill(getpid(), SIGKILL);
    } else if (SSL_ERROR_SYSCALL) {
        perror("SSL_ERROR_SYSCALL");
        SSL_shutdown(ssl);
        kill(getpid(), SIGKILL);
    }
}

int readFromSocket(int socketFd, unsigned char *readData, int readLen) {
    return recv(socketFd, readData, readLen, 0);
}


int writeToSocket(int socketFd, const unsigned char *writeData, int writeLen) {
    return send(socketFd, writeData, writeLen, 0);
}


int greetServer(const int socketFd, const unsigned char *username, const unsigned char *password) {
    int outSize = 0, readLen;
    char readData[SMTP_MTU] = {0};
    char writeData[SMTP_MTU] = {0};
    char userName[MAX_EMAIL_LEN] = {0};
    char userPasswd[MAX_EMAIL_LEN] = {0};
}



