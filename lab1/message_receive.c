#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>
#include "message_queue.h"

int main()
{
    int msqid = init_queue();
    int type = 0; 
    size_t maxBytes = 5; 
    ssize_t msgLen;
    struct mbuf msg; 

    msgLen = msgrcv(msqid, &msg, maxBytes, type, MSG_NOERROR | IPC_NOWAIT);

    if (msgLen == -1) {
        if (errno == ENOMSG) {
            printf("No message of type %d available in the queue (non-blocking).\n", type);
        } else {
            fprintf(stderr, "msgrcv error: %s (errno: %d)\n", strerror(errno), errno);
        }
        exit(-1);
    }

    printf("Received: type=%ld; length=%ld (truncated if > %zu bytes)", msg.mtype, (long) msgLen, maxBytes);

    if (msgLen > 0) {
        printf("; body=%.*s", (int)msgLen, msg.mtext);
    }

    printf("\n");
    exit(EXIT_SUCCESS);
}