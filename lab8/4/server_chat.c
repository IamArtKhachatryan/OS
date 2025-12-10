#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ipv4_dg.h"   /* PORT_NUM, BUF_SIZE, errExit */

int main(void)
{
    int listener;              /* լսող (server) սոկետ */
    int fdmax;                 /* max ֆայլի դեսկրիպտոր select-ի համար */
    fd_set master;             /* բոլոր ակտիվ սոկետների հավաքածու */
    fd_set read_fds;           /* select-ի համար temp հավաքածու */

    struct sockaddr_in svaddr, claddr;
    socklen_t addrlen;
    int yes = 1;
    char buf[BUF_SIZE];
    int i, j;

    /* 1) Ստեղծում ենք TCP հոսքային սոկետ */

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == -1) {
        errExit("socket");
    }

    /* SO_REUSEADDR, որ bind-ի ժամանակ "Address already in use" չտա վերագործարկելուց հետո */

    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        errExit("setsockopt");
    }

    /* 2) Լրացնում ենք server-ի հասցեն */

    memset(&svaddr, 0, sizeof(svaddr));
    svaddr.sin_family      = AF_INET;
    svaddr.sin_addr.s_addr = htonl(INADDR_ANY);   /* բոլոր ինտերֆեյսների վրա */
    svaddr.sin_port        = htons(PORT_NUM);     /* օրինակ՝ 50002 */

    if (bind(listener, (struct sockaddr *) &svaddr, sizeof(svaddr)) == -1) {
        errExit("bind");
    }

    if (listen(listener, SOMAXCONN) == -1) {
        errExit("listen");
    }

    printf("Chat server listening on port %d ...\n", PORT_NUM);

    /* 3) Նախնական FD_SET-երի_INIT */

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    FD_SET(listener, &master);    /* master-ում ավելացնում ենք listen սոկետը */
    fdmax = listener;             /* մինչ այժմ ամենամեծ fd-ն listener-ն է */

    /* 4) Գլխավոր անսահման ցիկլ */

    for (;;) {
        read_fds = master;        /* select-ը կփոխի read_fds-ը, master-ը կպահենք */

        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            errExit("select");
        }

        /* Անցնում ենք բոլոր fd-ներով, որ տեսնենք որներն են պատրաստ կարդալու */

        for (i = 0; i <= fdmax; i++) {
            if (!FD_ISSET(i, &read_fds))
                continue;

            if (i == listener) {
                /* Նոր միացում */

                addrlen = sizeof(claddr);
                int newfd = accept(listener, (struct sockaddr *) &claddr, &addrlen);
                if (newfd == -1) {
                    perror("accept");
                    continue;
                }

                FD_SET(newfd, &master);  /* նոր client-ը ավելացնում ենք master set-ի մեջ */
                if (newfd > fdmax) {
                    fdmax = newfd;
                }

                printf("New client on socket %d (%s:%d)\n",
                       newfd,
                       inet_ntoa(claddr.sin_addr),
                       ntohs(claddr.sin_port));


            } else {
                /* Արդեն գոյություն ունեցող client-ից տվյալներ ենք ստանում */

                ssize_t nbytes = recv(i, buf, sizeof(buf), 0);
                if (nbytes <= 0) {
                    /* 0 → client-ը փակեց կապը, <0 → սխալ */

                    if (nbytes == 0) {
                        printf("Socket %d disconnected\n", i);
                    } else {
                        perror("recv");
                    }

                    close(i);
                    FD_CLR(i, &master);
                } else {
                    /* nbytes > 0 → broadcast ենք անում բոլոր մյուս client-ներին */

                    /* Կարող ես ցանկության դեպքում ավելացնել prefix,
                       օրինակ "Client X: "․ հիմա ուղարկում ենք ինչ կա՝ ուղիղ */

                    for (j = 0; j <= fdmax; j++) {
                        if (FD_ISSET(j, &master)) {
                            /* չենք ուղարկում listen սոկետին ու նույն աղբյուր client-ին */
                            if (j != listener && j != i) {
                                if (send(j, buf, nbytes, 0) == -1) {
                                    perror("send");
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /* Թեև այստեղ չենք հասնի */
    close(listener);
    return 0;
}
