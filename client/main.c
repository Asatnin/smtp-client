#include <stdio.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <sys/queue.h>
#include <time.h>
#include <poll.h>

#include "smtp.h"
#include "network_helper.h"
#include "files_crawler.h"
#include "common_structs.h"
#include "converter.h"
#include "logger.h"
#include "client.h"

#define LOG_QUEUE_NAME "/log-queue"
#define QUEUE_PERMISSIONS 0660
#define MAX_MSGS 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

#define MAX_SOCKET_CONN

int main() {
//    char *email = "schepych@gmail.com";
    char *email = "garedok@gmail.com";
    char **mxhosts = getMXRecords(email);
    int i = -1;
    int max_connection = 0;
    Connection connections[MAX_SOCKET_CONN] = { };
    struct pollfd fds = {0};

    char *dirName = "/home/andrey/Development/smtp/client/example_maildir/";
    start_work(dirName, 0);

    TxtMail mail = {0};
    mail.from = "schepych@gmail.com";
    mail.to = "aazavalin@yandex.ru";
    mail.subject = "Тестовая отпрвака письма";
    mail.data = "Тестовая отпрвака письма";

    int server = connectToServer("mx.yandex.ru", 25);
    send_mail(server, &mail);

    return 0;

    mqd_t logger;
    char logger_name[30];
    strcpy(logger_name, LOG_NAME);


    // process id for child logging process
    pid_t processId;
    processId = fork();

    if (processId == 0) {
        logger = mq_open(logger_name, O_RDONLY);
        logger_init(logger, logger_name);
        return 0;
    } else {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_msgsize = LOG_SIZE;
        attr.mq_maxmsg = MAX_MQ_SIZE;
        attr.mq_curmsgs = 0;
        mq_unlink(logger_name);
        logger = mq_open(logger_name, O_CREAT | O_EXCL | O_NONBLOCK | O_WRONLY, 0777, &attr);
    }

    do_log(logger, ERROR, "Error on creating server socket, code %d", errno);

    HostnameList hostnameList;
    LIST_INIT(&hostnameList.node);

    char **files = listFiles(dirName, 5);
    while (files[++i] != NULL) {
        char *content = processFile(dirName, files[i]);
        printf("%s\n", content);

        TxtMail *mail = convertToMail(content);
        insert_mail_to_hostname_list(mail, &hostnameList);

        free(content);
        free(files[i]);
    }
    free(files);

    // try to find existing socket connection and send mails
    Hostname *hostname_ptr = hostnameList.node.lh_first;
    while (hostname_ptr) {
        if (LIST_EMPTY(&hostname_ptr->mail_list)) {
            hostname_ptr = hostname_ptr->hostname_pointers.le_next;
        }

        int found = 0;
        for (i = 0; i < max_connection; i++) {
            if (strcmp(connections[i].hostname, hostname_ptr->hostname)) {
                found = 1;
                break;
            }
        }
        // if no connection found we'll try to create one
        if (found == 0) {
            Connection conn;
            memset(&conn, 0, sizeof(conn));

            conn.hostname = hostname_ptr->hostname;
            char **conn_mx_hosts = getMXRecords(conn.hostname);
            conn.mx_hostname = conn_mx_hosts[0];
            conn.timeout = 10000;

            struct timespec spec;
            clock_gettime(CLOCK_REALTIME, &spec);
            conn.lat = spec.tv_sec * 1000L;

            conn.socket_fd = connectToServer(conn.mx_hostname, 25);

            connections[max_connection++] = conn;

            i = max_connection - 1;

            free(conn_mx_hosts);
        }

        // try to send all mail for found hostname
        Connection cur_conn = connections[i];
        TxtMailNode *mail_node = hostname_ptr->mail_list.lh_first;
        while (mail_node) {
            mail_node = mail_node->pointers.le_next;
        }

        hostname_ptr = hostname_ptr->hostname_pointers.le_next;
    }


//    if (processId == 0) {
//        printf("Hello from child process\n");
//
//        mqd_t qd_log;
//        struct mq_attr attr;
//        attr.mq_flags = 0;
//        attr.mq_maxmsg = MAX_MSGS;
//        attr.mq_msgsize = MAX_MSG_SIZE;
//        attr.mq_curmsgs = 0;
//
//        char in_buffer[MSG_BUFFER_SIZE];
//
//        if ((qd_log = mq_open(LOG_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
//            perror("Cannot create log receiving message queue. Logger will now exit.");
//            exit(1);
//        }
//
//        while (1) {
//            if ((mq_receive(qd_log, in_buffer, MSG_BUFFER_SIZE, NULL) != -1)) {
//                printf("Log msg: %s\n", in_buffer);
//            }
//        }
//
//        exit(0);
//    }
//
//    printf("Hello from parent process\n");
//
//    mqd_t qd_client = -1; // client queue descriptor
//
//    char out_buffer[MSG_BUFFER_SIZE];
//    int count = 0;
//
//    while (1) {
//        if (qd_client == -1) {
//            if ((qd_client = mq_open(LOG_QUEUE_NAME, O_WRONLY)) == -1) {
//                continue;
//            }
//        }
//
//        if (count > 56) {
//            break;
//        }
//
//        sprintf(out_buffer, "%d", count++);
//        mq_send(qd_client, out_buffer, strlen(out_buffer), 0);
//    }
//
//    mq_unlink(LOG_QUEUE_NAME);
//    sleep(10);

//    sendMail(mxhosts[0],
//             25,
//             "schepych@gmail.com",
//             email,
//             0,
//             "smtp test",
//             "smtp test",
//             "<h1>Smtp test</h1>"
//    );
//    int fd = connectToServer("mail.google.com", 25);
    return 0;
}