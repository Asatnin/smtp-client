//
// Created by andrey on 14.01.18.
//

#ifndef CLIENT_COMMON_STRUCTS_H
#define CLIENT_COMMON_STRUCTS_H

#include <sys/queue.h>

#include "str_helper.h"

typedef struct TxtMail {
    char *from;
    char *to;
    char *subject;
    char *data;
} TxtMail;

typedef struct Connection {
    char *mx_hostname;
    int socket_fd;
    long timeout;
    long lat;
} Connection;


typedef struct TxtMailNode {
    TxtMail *txt_mail;

    LIST_ENTRY(TxtMailNode) pointers;
} TxtMailNode;


typedef struct Hostname {
    char *hostname;

//    LIST_HEAD(txt_mail_list, TxtMailNode) mail_list;
    LIST_ENTRY(Hostname) hostname_pointers;
} Hostname;

typedef struct HostnameList {
    LIST_HEAD(hostname_list, Hostname) node;
} HostnameList;


void insert_mail_to_hostname_list(TxtMail *mail, HostnameList *list);

#endif //CLIENT_COMMON_STRUCTS_H
