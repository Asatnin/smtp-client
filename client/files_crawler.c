//
// Created by andrey on 14.01.18.
//

#include "files_crawler.h"
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void mark_locked(char *file_name, char *ext) {
    char path[MAX_PATH_LEN];
    int path_len = snprintf(path, sizeof(path) - 1, "%s%s", file_name, ext);
    path[path_len] = '\0';
    int fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
    close(fd);
}

int file_exists(char *file_name, char *ext) {
    char path[MAX_PATH_LEN];
    int path_len = snprintf(path, sizeof(path) - 1, "%s%s", file_name, ext);
    path[path_len] = '\0';
    struct stat buf;
    int res = stat(path, &buf);
    return res == 0;
}

int file_has_ext(char *file_name, char *ext) {
    char *dot_str = strrchr(file_name, '.');
    return dot_str && !strcmp(dot_str, ext);
}

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
            if (dir->d_type != DT_REG) {
                continue;
            }

            char path[MAX_PATH_LEN];
            int path_len = snprintf(path, sizeof(path) - 1, "%s%s", dirName, dir->d_name);
            path[path_len] = '\0';

            if (file_has_ext(path, ".tmp") || file_has_ext(path, ".lck")) {
                continue;
            }

            if (file_exists(path, ".lck")) {
                continue;
            }

            files[count] = malloc(sizeof(char) * strlen(dir->d_name));
            strcpy(files[count++], dir->d_name);
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

        mark_locked(path, ".lck");

        fclose(file);
    }

    free(path);

    return buf;
}