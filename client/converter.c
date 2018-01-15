//
// Created by andrey on 14.01.18.
//

#include "converter.h"

TxtMail* convertToMail(char *text) {
    int fromPos = 0, toPos = 0, subjectPos = 0, dataPos = 0, i = 0;
    int from_head_len = 6, to_head_len = 4, subject_head_len = 9;
    char endLine = '\n';

    TxtMail *result;
    result = (TxtMail*) malloc(sizeof(TxtMail));

    // read From: line
    while (text[i] != endLine) {
        fromPos = ++i;
    }
    int from_len = fromPos - from_head_len;
    result->from = malloc(sizeof(char) * (from_len + 1));
    memcpy(result->from, text + from_head_len, from_len);
    result->from[from_len] = '\0';

    // read To: line
    i++;
    while (text[i] != endLine) {
        toPos = ++i;
    }
    int to_len = toPos - fromPos - to_head_len - 1;
    result->to = malloc(sizeof(char) * (to_len + 1));
    memcpy(result->to, text + fromPos + to_head_len + 1, to_len);
    result->to[to_len] = '\0';

    // read Subject: line
    i++;
    while (text[i] != endLine) {
        subjectPos = ++i;
    }
    int subject_len = subjectPos - toPos - subject_head_len - 1;
    result->subject = malloc(sizeof(char) * (subject_len + 1));
    memcpy(result->subject, text + toPos + subject_head_len + 1, subject_len);
    result->subject[subject_len] = '\0';

    // read Data
    i++;
    int data_len = strlen(text) - i;
    result->data = malloc(sizeof(char) * (data_len + 1));
    memcpy(result->data, text + i, data_len);
    result->data[data_len] = '\0';

    return result;
}