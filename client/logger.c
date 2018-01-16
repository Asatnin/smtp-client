//
// Created by andrey on 16.01.18.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>

#include "logger.h"

char* get_mode(int mode) {
    if (mode == WARN)
        return "WARN";
    if (mode == ERROR)
        return "ERRR";
    return "INFO";
}

void logger_init(mqd_t logger, char* logger_name) {
    FILE* fp;
    int mode, income;
    char log_msg[LOG_SIZE], logfile[80];
    log_msg[0] = '\0';

    if (access(LOGDIR, 0) == -1) {
        mkdir(LOGDIR, 0777);
    }

    sprintf(logfile, LOGDIR "/smtp_log_%d.log", (int) time(NULL));
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
    mq_unlink(logger_name);
}

void do_log(mqd_t logger, int mode, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char* log_msg = (char*) malloc(LOG_SIZE * sizeof(char));
    vsprintf(log_msg, fmt, ap);
    mq_send(logger, log_msg, strlen(log_msg), mode);
    va_end(ap);
    free(log_msg);
}