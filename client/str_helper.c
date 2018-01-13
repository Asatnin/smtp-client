//
// Created by osboxes on 1/5/18.
//

#include "str_helper.h"

char* str_split(char* a_str, const char a_delim) {
    int splitIndex = 0;
    for (int i = 0; i < strlen(a_str); ++i) {
        if (a_str[i] == a_delim) {
            splitIndex = i;
            break;
        }
    }
    if (splitIndex == 0) {
        return NULL;
    }

    char *result = (char *) malloc(sizeof(char) * (strlen(a_str) - splitIndex)); // no -1 for \0-terminator
    memset(result, 0, sizeof(result));
    strncpy(result, a_str + splitIndex + 1, strlen(a_str) - splitIndex - 1);
    result[strlen(a_str) - splitIndex] = '\0';

    return result;
}

int str_contains(char *text, char *pattern) {
    return strstr(text, pattern) != NULL;
}
