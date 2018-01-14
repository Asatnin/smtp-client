//
// Created by andrey on 14.01.18.
//

#ifndef CLIENT_FILES_CRAWLER_H
#define CLIENT_FILES_CRAWLER_H

#include <dirent.h>
#include <memory.h>
#include <stdlib.h>

char** listFiles(char *dirName, int maxCount);
char* processFile(char *dirName, char *fileName);

#endif //CLIENT_FILES_CRAWLER_H
