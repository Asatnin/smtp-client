//
// Created by osboxes on 1/5/18.
//

#ifndef CLIENT_NETWORK_HELPER_H
#define CLIENT_NETWORK_HELPER_H


#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <resolv.h>

#include <netdb.h>

#include "str_helper.h"

#define N 4096


char** getMXRecords(char *email);

#endif //CLIENT_NETWORK_HELPER_H
