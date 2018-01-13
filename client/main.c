#include <stdio.h>
#include <sys/wait.h>

#include "smtp.h"
#include "network_helper.h"

int main() {
//    char *email = "schepych@gmail.com";
    char *email = "garedok@gmail.com";
    char **mxhosts = getMXRecords(email);
    int i = 2;


    // process id for child logging process
    pid_t processId;


//    sendMail(mxhosts[0],
//             25,
//             "schepych@gmail.com",
//             email,
//             0,
//             "smtp test",
//             "smtp test",
//             "<h1>Smtp test</h1>"
//    );
//    int fd = connectToServer("mail.google.com", 25);
    return 0;
}