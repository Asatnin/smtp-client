//
// Created by andrey on 14.01.18.
//

#include "files_crawler.h"
#include <stdio.h>

char** listFiles(char *dirName, int maxCount) {
    DIR *d;
    struct dirent *dir;

    int count = 0;

    char **files;
    files = malloc(sizeof(char*) * maxCount);
    memset(files, 0, sizeof(files));

    d = opendir(dirName);
    if (d) {
        while ((dir = readdir(d)) != NULL && count < maxCount) {
            if (dir->d_type == DT_REG) {
                files[count] = malloc(sizeof(char) * strlen(dir->d_name));
                strcpy(files[count++], dir->d_name);
            }
        }
        closedir(d);
    }

    return files;
}

char* processFile(char *dirName, char *fileName) {
    char *buf = NULL;
    FILE *file;

    char *path = malloc(sizeof(char) * (strlen(dirName) + strlen(fileName)));
    strcpy(path, dirName);
    strcpy(path + strlen(dirName), fileName);

    file = fopen(path, "r");
    if (file) {
        fseek(file, 0, SEEK_END);
        long strSize = ftell(file);
        buf = (char*) malloc(sizeof(char) * (strSize + 1));
        rewind(file);

        size_t readSize = fread(buf, sizeof(char), strSize, file);
        buf[strSize] = '\0';
        if (strSize != readSize) {
            free(buf);
            return NULL;
        }

//        remove(path);
        fclose(file);
    }

    free(path);

    return buf;
}