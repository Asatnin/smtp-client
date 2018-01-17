//
// Created by andrey on 14.01.18.
//

#ifndef CLIENT_COMMON_STRUCTS_H
#define CLIENT_COMMON_STRUCTS_H

#include <sys/queue.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

#include "str_helper.h"

#define MAX_CONN_LEN 20

typedef struct TxtMail {
    char *from;
    char *to;
    char *subject;
    char *data;
} TxtMail;

typedef struct Connection {
    char *hostname;
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

    LIST_HEAD(txt_mail_list, TxtMailNode) mail_list;
    LIST_ENTRY(Hostname) hostname_pointers;
} Hostname;

typedef struct HostnameList {
    LIST_HEAD(hostname_list, Hostname) node;
} HostnameList;



typedef struct ServerConnection {
    char *hostname;
    char *mx_hostname;

    struct sockaddr_in serv_addr;
    int pollfd_number;

    char *buf;
    int buffer_len;
} ServerConnection;


typedef struct Client {
    long lat;
    ServerConnection conns[MAX_CONN_LEN];
    int conns_len;
    struct pollfd fds[MAX_CONN_LEN];
    int fds_len;
} Client;



void insert_mail_to_hostname_list(TxtMail *mail, HostnameList *list);
void create_connection(ServerConnection *conn, char *hostname, int pollfd);

#endif //CLIENT_COMMON_STRUCTS_H
