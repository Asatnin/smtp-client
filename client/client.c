//
// Created by andrey on 17.01.18.
//

#include <sys/queue.h>
#include <stdio.h>
#include <unistd.h>
#include "client.h"
#include "common_structs.h"
#include "files_crawler.h"
#include "converter.h"

void prepare_connections(Client *client, HostnameList *list) {
    int i;
    Hostname *hostname_ptr = list->node.lh_first;
    while (hostname_ptr) {

        int found = 0;
        for (i = 0; i < client->conns_len; i++) {
            if (strcmp(hostname_ptr->hostname, client->conns[i].hostname) == 0) {
                // found existing server connection
                found = 1;
                break;
            }
        }
        if (!found) {
            // create new connection
            create_connection(&client->conns[client->conns_len], hostname_ptr->hostname, client->fds_len);
            client->conns_len++;

            struct pollfd pfd;
            pfd.fd = -1;

            client->fds[client->fds_len] = pfd;
            client->fds_len++;
        }

        hostname_ptr = hostname_ptr->hostname_pointers.le_next;
    }
}

int read_mail_directory(char *dirName, HostnameList *hostnameList) {
    int i = -1;
    char **files = listFiles(dirName, 5);
    while (files[++i] != NULL) {
        char *content = processFile(dirName, files[i]);
        printf("%s\n", content);

        TxtMail *mail = convertToMail(content);
        insert_mail_to_hostname_list(mail, hostnameList);

        free(content);
        free(files[i]);
    }
    free(files);

    return i;
}

void start_work(char *dirName) {
    HostnameList hostnameList;
    LIST_INIT(&hostnameList.node);

    Client client;
    memset(&client, 0, sizeof(client));

    while (1) {
        int read_files = read_mail_directory(dirName, &hostnameList);
        if (read_files == 0) {
            sleep(3);
            continue;
        }

        prepare_connections(&client, &hostnameList);

        int has_next_mail = 1;
        while (has_next_mail) {
            has_next_mail = 0;
        }
    }
}
