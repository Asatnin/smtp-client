//
// Created by andrey on 17.01.18.
//

#include <sys/queue.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#include <mqueue.h>
#include "client.h"
#include "common_structs.h"
#include "files_crawler.h"
#include "converter.h"
#include "smtp.h"

void *do_send_mail(void *input);

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

void make_connections_active(Client *client) {
    int i;
    struct hostent *host = NULL;

    for (i = 0; i < client->conns_len; i++) {
        struct pollfd *fd = &client->fds[client->conns[i].pollfd_number];
        if (fd->fd == -1) {
            fd->fd = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
            fd->events = POLLIN; // wait to start read from
            if (fd->fd < 0) {
                fd->fd = -1;
                continue;
            }
        }

        if ((host = gethostbyname(client->conns[i].mx_hostname)) == NULL) {
            perror("error while calling gethostbyname");
            exit(-1);
        }

        memset(&client->conns[i].serv_addr, 0, sizeof(client->conns[i].serv_addr));
        client->conns[i].serv_addr.sin_family = AF_INET;
        client->conns[i].serv_addr.sin_port = htons(25);
        client->conns[i].serv_addr.sin_addr = *((struct in_addr *)host->h_addr);

        int cr = connect(fd->fd, (struct sockaddr *)&client->conns[i].serv_addr, sizeof(client->conns[i].serv_addr));
        if (cr == 0 || (cr == -1 && errno == EINPROGRESS)) {
            // success or waiting
        } else {
            // error
            fd->fd = -1;
        }
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

void start_work(char *dirName, mqd_t logger) {
    int i, j;

    HostnameList hostnameList;
    LIST_INIT(&hostnameList.node);

    Client client;
    memset(&client, 0, sizeof(client));

    while (1) {
        int read_files = read_mail_directory(dirName, &hostnameList);

        prepare_connections(&client, &hostnameList);
        make_connections_active(&client);

        int pollr = poll(client.fds, client.fds_len, POLL_TIMEOUT);
        if (pollr > 0) {
            // lets find socket thats ready to rumble
            ServerConnection *go_conn;
            for (i = 0; i < client.fds_len; i++) {
                if (client.fds[i].revents > 0) {
                    // wow, it's READY, find connection
                    for (j = 0; j < client.conns_len; j++) {
                        if (client.conns[j].pollfd_number == i) {
                            // finally we found connection thats READY
                            go_conn = &client.conns[j];
                            break;
                        }
                    }

                    // now find mail queue for given hostname
                    Hostname *hostname_ptr = hostnameList.node.lh_first;
                    while (hostname_ptr) {
                        if (strcmp(hostname_ptr->hostname, go_conn->hostname) == 0) {
                            break;
                        }
                        hostname_ptr = hostname_ptr->hostname_pointers.le_next;
                    }

                    if (hostname_ptr == NULL) {
                        continue;
                    }

                    pthread_t con_thread;
                    ThreadHandler th;
                    th.socket = &client.fds[i].fd;
                    th.socket_id = i;
                    th.logger = &logger;
                    th.hostname_mail_list = hostname_ptr;

                    LIST_REMOVE(hostname_ptr, hostname_pointers);

                    if (pthread_create(&con_thread, NULL, do_send_mail, (void*) &th) < 0) {
                        printf("err when create thread\n");
                        continue;
                    }
                    pthread_detach(con_thread);
                }
            }
        }

        int has_next_mail = 1;
        while (has_next_mail) {
            has_next_mail = 0;
        }
    }
}

void *do_send_mail(void *input) {
    ThreadHandler th = *((ThreadHandler*) input);
    int con = *th.socket;
    mqd_t logger = *th.logger;
    Hostname *hostname_mail_list = th.hostname_mail_list;

    // greet server first
    int greet_res = greet_server(con);
    if (greet_res != 0) {
        printf("err while greet\n");
        return NULL;
    }

    TxtMailNode *mail_node = hostname_mail_list->mail_list.lh_first;
    while (mail_node) {
//        send
        int send_res = send_mail(con, mail_node->txt_mail);
        if (send_res != 0) {
            printf("smth bad happened\n");
        }
        mail_node = mail_node->pointers.le_next;
    }

    int bye_status = bye_server(con);
    if (bye_status != 0) {
        printf("err while quit\n");
        return NULL;
    }
}
