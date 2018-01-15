//
// Created by osboxes on 1/5/18.
//

#include "network_helper.h"

char** getMXRecords(char *email) {
    char *domain = "localhost";
    char *splits = str_split(email, '@'); // do not forget to clean
    char **mxhosts; // do not forget to clean

    if (splits != NULL) {
        domain = splits;
    }

    res_init();
    u_char nsbuf[N];
    ns_msg msg;
    ns_rr rr;
    int i, l;

    printf("Getting MX records: \n");

    l = res_query(domain, ns_c_any, ns_t_mx, nsbuf, sizeof(nsbuf));
//    free(splits);
    if (l < 0) {
        return NULL;
    }

    ns_initparse(nsbuf, l, &msg);
    l = ns_msg_count(msg, ns_s_an);
    mxhosts = malloc(sizeof(char *) * l);
    for (i = 0; i < l; i++) {
        ns_parserr(&msg, ns_s_an, i, &rr);

        char exchange[NS_MAXDNAME];
        const u_char *rdata = ns_rr_rdata(rr);
        const uint16_t pri = ns_get16(rdata);
        int len = dn_expand(nsbuf, nsbuf + 250, rdata + 2, exchange, sizeof(exchange));

        mxhosts[i] = malloc(sizeof(char) * strlen(exchange));
        strcpy(mxhosts[i], exchange);

        // priority
        printf("MX-record priority:%d\t", pri);
        // hostname
        printf("hostname:%s\n", mxhosts[i]);
    }

    return mxhosts;
}