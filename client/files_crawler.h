//
// Created by andrey on 14.01.18.
//

#ifndef CLIENT_FILES_CRAWLER_H
#define CLIENT_FILES_CRAWLER_H

#include <dirent.h>
#include <memory.h>
#include <stdlib.h>

#define MAX_PATH_LEN 128

void mark_locked(char *file_name, char *ext);
char** listFiles(char *dirName, int maxCount);
char* processFile(char *dirName, char *fileName);

#endif //CLIENT_FILES_CRAWLER_H
