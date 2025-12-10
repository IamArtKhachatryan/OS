#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ipv4_dg.h"   /* PORT_NUM, BUF_SIZE, errExit */

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <server-ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sfd;
    struct sockaddr_in svaddr;
    char buf[BUF_SIZE];
    fd_set read_fds;
    int fdmax;

    /* 1) Ստեղծում ենք TCP սոկետ */

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        errExit("socket");
    }

    /* 2) Լրացնում ենք server-ի հասցեն */

    memset(&svaddr, 0, sizeof(svaddr));
    svaddr.sin_family = AF_INET;
    svaddr.sin_port   = htons(PORT_NUM);

    if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0) {
        errExit("inet_pton");
    }

    /* ՈւՇԱԴՐՈՒԹՅՈՒՆ: client կողմում bind() չի պետք */

    if (connect(sfd, (struct sockaddr *) &svaddr, sizeof(svaddr)) == -1) {
        errExit("connect");
    }

    printf("Connected to chat server %s:%d\n", argv[1], PORT_NUM);
    printf("Type messages and press Enter to send. Ctrl+D or Ctrl+C to exit.\n");

    /* 3) Գլխավոր ցիկլ․ լսում ենք թե՛ stdin, թե՛ socket */

    for (;;) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);  /* user input */
        FD_SET(sfd, &read_fds);           /* server-ից եկող տվյալներ */

        fdmax = (sfd > STDIN_FILENO) ? sfd : STDIN_FILENO;

        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            errExit("select");
        }

        /* 1) ստուգում ենք՝ user-ը ինչ-որ բան գրե՞լ է */

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            /* եթե EOF (Ctrl+D), ապա դուրս ենք գալիս */

            if (fgets(buf, sizeof(buf), stdin) == NULL) {
                printf("EOF on stdin, exiting...\n");
                break;
            }

            size_t len = strlen(buf);
            if (len > 0) {
                if (send(sfd, buf, len, 0) == -1) {
                    errExit("send");
                }
            }
        }

        /* 2) ստուգում ենք՝ սոկետից մի բան եկե՞լ է */

        if (FD_ISSET(sfd, &read_fds)) {
            ssize_t nbytes = recv(sfd, buf, sizeof(buf) - 1, 0);
            if (nbytes <= 0) {
                if (nbytes == 0) {
                    printf("Server closed the connection.\n");
                } else {
                    perror("recv");
                }
                break;
            }

            buf[nbytes] = '\0';
            printf(">> %s", buf);   /* արդեն newline-ի դեպքում կդրվի տողի վրա */
            fflush(stdout);
        }
    }

    close(sfd);
    return 0;
}
