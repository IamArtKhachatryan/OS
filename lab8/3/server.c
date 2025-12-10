#include <string.h>
#include <unistd.h>
#include "ipv4_dg.h"      /* BUF_SIZE, PORT_NUM, errExit */

int main(void)
{
    int lfd, cfd;
    struct sockaddr_in svaddr, claddr;
    socklen_t len;
    char buf[BUF_SIZE];
    ssize_t numRead;
    int optval = 1;

    /* 1) Ստեղծում ենք հոսքային (TCP) սոկետ */

    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        errExit("socket");
    }

    /* SO_REUSEADDR, որ bind-ի ժամանակ խնդիր չլինի վերագործարկելուց հետո */

    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        errExit("setsockopt");
    }

    /* 2) Լրացնում ենք server-ի հասցեն */

    memset(&svaddr, 0, sizeof(svaddr));
    svaddr.sin_family      = AF_INET;
    svaddr.sin_addr.s_addr = htonl(INADDR_ANY);   /* լսում է բոլոր ինտերֆեյսների վրա */
    svaddr.sin_port        = htons(PORT_NUM);     /* 50002, ըստ ipv4_dg.h-ի */

    /* 3) Կապում ենք սոկետը հասցեին (bind) */

    if (bind(lfd, (struct sockaddr *) &svaddr, sizeof(svaddr)) == -1) {
        errExit("bind");
    }

    /* 4) Դնում ենք listen վիճակի */

    if (listen(lfd, SOMAXCONN) == -1) {
        errExit("listen");
    }

    printf("Echo TCP server listening on port %d ...\n", PORT_NUM);

    /* 5) Հիմնական ցիկլ․ սպասում ենք client-ների և սպասարկում հերթով */

    for (;;) {
        len = sizeof(claddr);
        cfd = accept(lfd, (struct sockaddr *) &claddr, &len);
        if (cfd == -1) {
            perror("accept");
            continue;  /* Չենք քսում server-ը ամբողջությամբ */
        }

        printf("Client connected.\n");

        /* Կարդում ենք տվյալները, և ինչ ստացանք՝ ուղարկում ենք հետ */

        for (;;) {
            numRead = recv(cfd, buf, BUF_SIZE, 0);
            if (numRead == -1) {
                errExit("recv");
            }

            if (numRead == 0) {
                /* client-ը փակեց կապը */
                printf("Client disconnected.\n");
                break;
            }

            if (send(cfd, buf, numRead, 0) != numRead) {
                errExit("send");
            }
        }

        close(cfd);
    }

    /* lfd չե՞նք փակում, որովհետև server-ը անվերջ է աշխատում */
    return 0;
}
