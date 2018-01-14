//
// Created by andrey on 14.01.18.
//

#include "converter.h"

TxtMail* convertToMail(char *text) {
    int fromPos, toPos, subjectPos, dataPos, i = 0;
    char endLine = '\n';

    TxtMail *result;
    result = (TxtMail*) malloc(sizeof(TxtMail));
    // now for test purpose
    result->from = "vasya@gmail.com";
    result->to = "aazavalin@yandex.ru";
    result->subject = "Smtp client test";
    result->data = "Here Is SMTP CLIENT TEST DATA TEXT";

    return result;
}