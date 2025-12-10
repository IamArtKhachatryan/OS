#include <string.h>
#include <unistd.h>
#include "ipv4_dg.h"      /* BUF_SIZE, PORT_NUM, errExit */

int main(int argc, char *argv[])
{
    int sfd;
    struct sockaddr_in svaddr;
    ssize_t numBytes;
    char buf[BUF_SIZE];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <server-ip> <message>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

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

    /* ՈւՇԱԴՐՈՒԹՅՈՒՆ: client կողմում bind() չի պետք, connect-ը ինքը հատկացնի port */

    if (connect(sfd, (struct sockaddr *) &svaddr, sizeof(svaddr)) == -1) {
        errExit("connect");
    }

    /* 3) Ուղարկում ենք argv[2]-ը server-ին */

    size_t msgLen = strlen(argv[2]);
    if (msgLen > BUF_SIZE) {
        msgLen = BUF_SIZE;   /* պարզության համար կտրում ենք */
    }

    if (send(sfd, argv[2], msgLen, 0) != (ssize_t) msgLen) {
        errExit("send");
    }

    /* 4) Սպասում ենք Echo պատասխանին */

    numBytes = recv(sfd, buf, BUF_SIZE, 0);
    if (numBytes == -1) {
        errExit("recv");
    }

    printf("Echo from server: %.*s\n", (int) numBytes, buf);

    close(sfd);
    return 0;
}
