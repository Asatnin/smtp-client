//
// Created on 1/3/18.
//

#include "smtp.h"

/*
 * return socket file descriptor after establishing connection to smtp server
 */
int connectToServer(const char *serverUrl, const unsigned short serverPort) {
    int socketFd = -1;
    struct sockaddr_in serverAddress = {0};
    struct hostent *host = NULL;
    int opt = 1;

    if ((host = gethostbyname(serverUrl)) == NULL) {
        perror("error while calling gethostbyname");
        exit(-1);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    serverAddress.sin_addr = *((struct in_addr *)host->h_addr);

    socketFd = socket(PF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        perror("socket creation error...\n");
        exit(-1);
    }

    if (connect(socketFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        close(socketFd);
        perror("connection error...\n");
        exit(-1);
    }

    return socketFd;
}

int parse_status(char *rcv_str) {
    char result_sts[4] = {0};

    memset(result_sts, 0, sizeof(result_sts));
    strncpy(result_sts, rcv_str, 3);

    int res = atoi(result_sts);

    return res;
}

int read_sckt(int socketFd, unsigned char *readData, int readLen) {
//    struct pollfd fds = {0};
//    int ret, len = 0, num_fds = 1, timeout = 5;
//
//    fds.fd = socketFd;
//    fds.events = POLLIN;
//
//    while (1) {
//        ret = poll(&fds, num_fds, timeout);
//        if (ret < 0) {
//            perror("poll called with error...\n");
//        } else if (ret == 0) {
//            continue;
//        }
//
//        if (fds.revents & POLLIN) {
//            len = recv(socketFd, readData, readLen, 0);
//        }
//
//        break;
//    }
//
//    return len;

    return recv(socketFd, readData, readLen, 0);
}


int write_sckt(int socketFd, const unsigned char *writeData, int writeLen) {
    return send(socketFd, writeData, writeLen, 0);
}

int greet_server(int socket_fd) {
    int sts;
    char read_data[SMTP_BUF_SIZE] = {0};

    // init read
    memset(&read_data, 0, SMTP_BUF_SIZE);
    read_sckt(socket_fd, read_data, SMTP_BUF_SIZE);
    printf("[%s][%d]recv: %s\r\n", __FILE__, __LINE__, read_data);

    // send ehlo
    write_sckt(socket_fd, "EHLO Here\r\n", strlen("EHLO Here\r\n"));

    // receive response to ehlo
    memset(&read_data, 0, SMTP_BUF_SIZE);
    read_sckt(socket_fd, read_data, SMTP_BUF_SIZE);
    printf("[%s][%d]recv: %s\r\n", __FILE__, __LINE__, read_data);
    sts = parse_status(read_data);
    if (sts != 250) {
        printf("cannot greet server due to bad status\r\n");
        return -1;
    }

    // greet is OK
    return 0;
}

int create_mail_text(unsigned char **mail, TxtMail *txt_mail) {
    char *mail_text = NULL;
    int mail_text_len = 0;
    char from_name[MAX_EMAIL_LEN] = {0};
    char to_name[MAX_EMAIL_LEN] = {0};

    if (*mail == NULL || txt_mail->from == NULL || txt_mail->to == NULL) {
        return -1;
    }

    mail_text_len = strlen(txt_mail->from)
                    + strlen(txt_mail->to)
                    + strlen(txt_mail->subject)
                    + strlen(txt_mail->data) + 500;

    mail_text = calloc(mail_text_len, 1);
    if (mail_text == NULL) {
        return -1;
    }

    memset(from_name, 0, MAX_EMAIL_LEN);
    strcpy(from_name, txt_mail->from);

    memset(to_name, 0, MAX_EMAIL_LEN);
    strcpy(to_name, txt_mail->to);

    snprintf(mail_text,
             mail_text_len,
             "From: <%s>\r\nTo: <%s>\r\nSubject: %s\r\nMIME-Version:1.0\r\nContent-Type: text/plain; charset=\"utf-8\"\r\n\r\n%s\r\n\r\n.\r\n",
             from_name, to_name, txt_mail->subject, txt_mail->data);

    *mail = realloc(*mail, strlen(*mail) + strlen(mail_text) + 1);

    strcat(*mail, mail_text);

    free(mail_text);

    return 0;
}


int send_mail(int server, TxtMail *mail) {
    int sts;
    char read_data[SMTP_BUF_SIZE] = {0};
    char write_data[SMTP_BUF_SIZE] = {0};

    sts = greet_server(server);
    if (sts != 0) {
        printf("error while greeting server\r\n");
        return -1;
    }

    // send mail from
    memset(&write_data, 0, SMTP_BUF_SIZE);
    sprintf(write_data, "MAIL FROM: <%s>\r\n", mail->from);
    write_sckt(server, write_data, strlen(write_data));


    // receive response from mail from
    memset(&read_data, 0, SMTP_BUF_SIZE);
    read_sckt(server, read_data, SMTP_BUF_SIZE);
    printf("[%s][%d]recv: %s\r\n", __FILE__, __LINE__, read_data);
    sts = parse_status(read_data);
    if (sts != 250) {
        printf("error while sending mail from\r\n");
        return -1;
    }


    // send rcpt to
    memset(&write_data, 0, SMTP_BUF_SIZE);
    sprintf(write_data, "RCPT TO: <%s>\r\n", mail->to);
    write_sckt(server, write_data, strlen(write_data));

    // receive response from rcpt to
    memset(&read_data, 0, SMTP_BUF_SIZE);
    read_sckt(server, read_data, SMTP_BUF_SIZE);
    printf("[%s][%d]recv: %s\r\n", __FILE__, __LINE__, read_data);
    sts = parse_status(read_data);
    if (sts != 250) {
        printf("error while sending rcpt to\r\n");
        return -1;
    }



    // send data
    memset(&write_data, 0, SMTP_BUF_SIZE);
    sprintf(write_data, "DATA\r\n");
    write_sckt(server, write_data, strlen(write_data));

    // receive response from data
    memset(&read_data, 0, SMTP_BUF_SIZE);
    read_sckt(server, read_data, SMTP_BUF_SIZE);
    printf("[%s][%d]recv: %s\r\n", __FILE__, __LINE__, read_data);
    sts = parse_status(read_data);
    if (sts != 354) {
        printf("error while sending data\r\n");
        return -1;
    }



    unsigned char *data = NULL;
    data = calloc(1, 1);
    sts = create_mail_text(&data, mail);
    if (sts == -1) {
        printf("invalid email\r\n");
        return -1;
    }



    // send mail text
    write_sckt(server, data, strlen(data));

    // receive response from mail text
    memset(&read_data, 0, SMTP_BUF_SIZE);
    read_sckt(server, read_data, SMTP_BUF_SIZE);
    printf("[%s][%d]recv: %s\r\n", __FILE__, __LINE__, read_data);
    sts = parse_status(read_data);
    if (sts != 250) {
        printf("error while sending mail text\r\n");
        return -1;
    }




    // send quit
    memset(&write_data, 0, SMTP_BUF_SIZE);
    write_sckt(server, "QUIT\r\n", strlen("QUIT\r\n"));

    // receive response from quit
    memset(&read_data, 0, SMTP_BUF_SIZE);
    read_sckt(server, read_data, SMTP_BUF_SIZE);
    printf("[%s][%d]recv: %s\r\n", __FILE__, __LINE__, read_data);
    sts = parse_status(read_data);
    if (sts != 221) {
        printf("error while quitting\r\n");
        return 0;
    }

    return 0;
}



