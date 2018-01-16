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
#include <poll.h>

#include "common_structs.h"

#define SMTP_BUF_SIZE 800
#define MAX_EMAIL_LEN 64
#define BUF_SIZE 8196
#define STR_LEN 32

int connectToServer(const char *serverUrl, const unsigned short serverPort);
int send_mail(int server, TxtMail *mail);

#endif //CLIENT_SMTP_H
