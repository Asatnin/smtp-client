//
// Created by andrey on 17.01.18.
//

#include <mqueue.h>

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#define POLL_TIMEOUT 500


void start_work(char *dirName, mqd_t logger);


#endif //CLIENT_CLIENT_H
