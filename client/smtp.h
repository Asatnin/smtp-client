//
// Created on 1/3/18.
//

#ifndef CLIENT_SMTP_H
#define CLIENT_SMTP_H

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <unistd.h>
#include <signal.h>

#define SMTP_MTU 800
#define MAX_EMAIL_LEN 64
#define BUF_SIZE 8196
#define STR_LEN 32

SSL_CTX* initSSLContext();
void sslError(SSL *ssl, int received);
int connectToServer(const char *serverUrl, const unsigned short serverPort);
int sendMail(char *hostname,
             const unsigned short portnum,
             char *from,
             char *to,
             char *replyto,
             char *subject,
             char *msg,
             char *msghtml);

#endif //CLIENT_SMTP_H
