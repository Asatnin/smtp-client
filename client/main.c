#include <stdio.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <sys/queue.h>
#include <time.h>
#include <poll.h>
#include <signal.h>

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

int main() {
    signal(SIGINT, exit_handler);
//    char *email = "schepych@gmail.com";
    char *email = "garedok@gmail.com";
    int i = -1;
    int max_connection = 0;
    Connection connections[MAX_SOCKET_CONN] = { };
    struct pollfd fds = {0};

    char *dirName = "/home/andrey/Development/smtp/client/maildir/";
    start_work(dirName, 0);

    return 0;

    TxtMail mail = {0};
    mail.from = "schepych@gmail.com";
    mail.to = "aazavalin@yandex.ru";
    mail.subject = "Тестовая отпрвака письма";
    mail.data = "Тестовая отпрвака письма";

    int server = connectToServer("mx.yandex.ru", 25);
    send_mail(server, &mail);
}