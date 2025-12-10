#include <sys/types.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "unix_stream.h"
#define BACKLOG 5

int main(int argc, char *argv[])
{
    struct sockaddr_un addr;
    int sfd, cfd;
    ssize_t numRead;
    char buf[BUF_SIZE];

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        fprintf(stderr, "socket error\n");
        exit(EXIT_FAILURE);
    }

    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
        fprintf(stderr, "remove-%s error", SV_SOCK_PATH);
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        if (errno == EADDRINUSE) {
            fprintf(stderr, "Address already in use\n");
        } else {
            fprintf(stderr, "bind error\n");
        }

        exit(EXIT_FAILURE);
    }

    if (listen(sfd, BACKLOG) == -1) {
        fprintf(stderr, "listen error\n");
        exit(EXIT_FAILURE);
    }

    int client_fds[FD_SETSIZE];
    fd_set readfds;
    int maxfd, i;

    for (i = 0; i < FD_SETSIZE; i++)
        client_fds[i] = -1;

    maxfd = sfd;

    for (;;) {
        FD_ZERO(&readfds);
        FD_SET(sfd, &readfds);          /* լսում ենք նոր միացումների համար */

        for (i = 0; i < FD_SETSIZE; i++) {
            if (client_fds[i] != -1)
                FD_SET(client_fds[i], &readfds);
        }

        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        /* Նոր client է միանում */
        if (FD_ISSET(sfd, &readfds)) {
            cfd = accept(sfd, NULL, NULL);
            if (cfd == -1) {
                perror("accept");
            } else {
                for (i = 0; i < FD_SETSIZE; i++) {
                    if (client_fds[i] == -1) {
                        client_fds[i] = cfd;
                        if (cfd > maxfd) maxfd = cfd;
                        printf("New client connected: fd=%d\n", cfd);
                        break;
                    }
                }
                if (i == FD_SETSIZE) {
                    fprintf(stderr, "too many clients\n");
                    close(cfd);
                }
            }
        }

        /* Տվյալները գոյություն ունեցող client-ներից */
        for (i = 0; i < FD_SETSIZE; i++) {
            int fd = client_fds[i];
            if (fd == -1) continue;

            if (FD_ISSET(fd, &readfds)) {
                numRead = read(fd, buf, BUF_SIZE);
                if (numRead <= 0) {
                    if (numRead == 0)
                        printf("Client %d disconnected\n", fd);
                    else
                        perror("read");
                    close(fd);
                    client_fds[i] = -1;
                    continue;
                }

                /* 🔥 CHATROOM – broadcast բոլոր մյուս client-ներին */
                for (int j = 0; j < FD_SETSIZE; j++) {
                    int other = client_fds[j];
                    if (other != -1 && other != fd) {
                        if (write(other, buf, numRead) != numRead) {
                            perror("write to client");
                        }
                    }
                }
            }
        }
    }
}
