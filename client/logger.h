//
// Created by andrey on 16.01.18.
//

#ifndef CLIENT_LOGGER_H
#define CLIENT_LOGGER_H

#include <mqueue.h>

#define INFO 0
#define WARN 1
#define ERROR 2
#define CONTROL 3
#define LOG_SIZE 128
#define MAX_MQ_SIZE 10 //why limited
#define LOG_NAME "/smtp-client-logger"
//#define LOGDIR "/var/log/smtp"
#define LOGDIR "/home/andrey/smtp/client-logging"

void do_log(mqd_t, int, const char* fmt, ...);
void logger_init(mqd_t, char*);
char* get_mode(int mode);

#endif //CLIENT_LOGGER_H
