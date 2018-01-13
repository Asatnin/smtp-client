#include <stdio.h>
#include <sys/wait.h>
#include <mqueue.h>

#include "smtp.h"
#include "network_helper.h"

#define LOG_QUEUE_NAME "/log-queue"
#define QUEUE_PERMISSIONS 0660
#define MAX_MSGS 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

int main() {
//    char *email = "schepych@gmail.com";
    char *email = "garedok@gmail.com";
    char **mxhosts = getMXRecords(email);
    int i = 2;


    // process id for child logging process
    pid_t processId;
    processId = fork();


    if (processId == 0) {
        printf("Hello from child process\n");

        mqd_t qd_log;
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = MAX_MSGS;
        attr.mq_msgsize = MAX_MSG_SIZE;
        attr.mq_curmsgs = 0;

        char in_buffer[MSG_BUFFER_SIZE];

        if ((qd_log = mq_open(LOG_QUEUE_NAME, O_RDONLY | O_CREAT, &attr)) == -1) {
            perror("Cannot create log receiving message queue. Logger will now exit.");
            exit(1);
        }

        while (1) {
            if ((mq_receive(qd_log, in_buffer, MSG_BUFFER_SIZE, NULL) != -1)) {
                printf("Log msg: %s\n", in_buffer);
            }
        }

        exit(0);
    }

    printf("Hello from parent process\n");

    mqd_t qd_client; // client queue descriptor

    char out_buffer[MSG_BUFFER_SIZE];

    while (1) {
        if ((qd_client = mq_open(LOG_QUEUE_NAME, O_WRONLY)) == -1) {
            continue;
        }

        sprintf(out_buffer, "hello");
        mq_send(qd_client, out_buffer, strlen(out_buffer), 0);
        break;
    }


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