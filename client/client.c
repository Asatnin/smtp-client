//
// Created by andrey on 17.01.18.
//

#include <sys/queue.h>
#include <stdio.h>
#include "client.h"
#include "common_structs.h"
#include "files_crawler.h"
#include "converter.h"

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

void start_work(char *dirName) {
//    char *dirName = "/home/andrey/Development/smtp/client/example_maildir/";
    HostnameList hostnameList;
    LIST_INIT(&hostnameList.node);


    while (1) {
        LIST_EMPTY(&hostnameList.node);

        read_mail_directory(dirName, &hostnameList);
    }
}
