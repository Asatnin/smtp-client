#include <stdio.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <sys/queue.h>
#include <time.h>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>

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

int is_run = 1;

static void exit_handler(int sig_num) {
    is_run = 0;
}

void logger_execute(mqd_t logger,const char* logdir)
{
    FILE* fp;
    int mode, income;
    char log_msg[LOG_SIZE], logfile[80];
    log_msg[0] = '\0';
    if (access(logdir, 0) == -1) {
        mkdir(logdir, 0777);
    }
    sprintf(logfile, "%ssmtp_log_%d.log", logdir, (int) time(NULL));
    do {
        income = mq_receive(logger, log_msg, LOG_SIZE, (u_int*) &mode);
        if (income > 0)
        {
            log_msg[income] = '\0';
            fp = fopen(logfile, "a");
            if (fp != NULL) {
                fprintf(fp, "%s: %s\n", get_mode(mode), log_msg);
                fclose(fp);
            }
        }
    } while (1);
    mq_close(logger);
}

int main() {
    signal(SIGINT, exit_handler);
//    char *email = "schepych@gmail.com";
    char *email = "garedok@gmail.com";
    int i = -1;
    int max_connection = 0;
    Connection connections[MAX_SOCKET_CONN] = { };
    struct pollfd fds = {0};
    char *log_dir = "/home/andrey/Development/smtp/logs/";

    mqd_t logger;
    char logger_name[30];
    strcpy(logger_name, LOG_NAME);
    if (fork() == 0) {
        logger = mq_open(logger_name,O_RDONLY);
        logger_execute(logger, log_dir);
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

    char *dirName = "/home/andrey/Development/smtp/client/maildir/";
    start_work(dirName, logger);

    return 0;
}