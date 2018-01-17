//
// Created by andrey on 14.01.18.
//

#include "common_structs.h"

void create_connection(ServerConnection *conn, char *hostname, int pollfd) {
    conn->hostname = hostname;
    conn->pollfd_number = pollfd;
}

void insert_mail_to_hostname_list(TxtMail *mail, HostnameList *list) {
    char *hostname = str_split(mail->to, '@');
    Hostname *hostname_ptr = list->node.lh_first;
    while (hostname_ptr) {
        if (strcmp(hostname_ptr->hostname, hostname) == 0) {
            break;
        }
        hostname_ptr = hostname_ptr->hostname_pointers.le_next;
    }

    TxtMailNode *txt_mail_node = malloc(sizeof(TxtMailNode));
    txt_mail_node->txt_mail = mail;

    if (hostname_ptr == NULL) {
        Hostname *node = malloc(sizeof(Hostname));
        node->hostname = hostname;
        LIST_INSERT_HEAD(&list->node, node, hostname_pointers);

        LIST_INIT(&node->mail_list);
        LIST_INSERT_HEAD(&node->mail_list, txt_mail_node, pointers);
    } else {
        LIST_INSERT_HEAD(&hostname_ptr->mail_list, txt_mail_node, pointers);
    }
}

