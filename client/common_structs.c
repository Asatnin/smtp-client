//
// Created by andrey on 14.01.18.
//

#include "common_structs.h"

void insert_mail_to_hostname_list(TxtMail *mail, HostnameList *list) {
    char *hostname = str_split(mail->to, '@');
    Hostname *hostname_ptr = list->node.lh_first;
    while (hostname_ptr) {
        if (strcmp(hostname_ptr->hostname, hostname) == 0) {
            break;
        }
        hostname_ptr = hostname_ptr->hostname_pointers.le_next;
    }

    if (hostname_ptr == NULL) {
        Hostname *node = malloc(sizeof(Hostname));
        node->hostname = hostname;
        LIST_INSERT_HEAD(&list->node, node, hostname_pointers);
    }

    int i = 2;
}

