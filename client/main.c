#include <stdio.h>
#include <sys/wait.h>
#include <mqueue.h>

#include "smtp.h"
#include "network_helper.h"
#include "files_crawler.h"

#define LOG_QUEUE_NAME "/log-queue"
#define QUEUE_PERMISSIONS 0660
#define MAX_MSGS 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

int main() {
//    char *email = "schepych@gmail.com";
    char *email = "garedok@gmail.com";
    char **mxhosts = getMXRecords(email);
    int i = -1;

    char *dirName = "/home/andrey/Development/smtp/client/example_maildir/";
    char **files = listFiles(dirName, 5);
    while (files[++i] != NULL) {
        char *content = processFile(dirName, files[i]);
        printf("%s\n", content);
        free(content);
        free(files[i]);
    }
    free(files);

    // process id for child logging process
    pid_t processId;
    processId = fork();


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