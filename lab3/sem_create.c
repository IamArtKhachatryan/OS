#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include "sem.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage error: the program accepts exactly one argument - the number of semaphores in the set.\n");
        exit(EXIT_FAILURE);
    }

    union semun arg;
    int n = atoi(argv[1]);
    int semid = semget(IPC_PRIVATE, n, S_IRUSR | S_IWUSR);

    if (semid == -1) {
        fprintf(stderr, "semid error\n");
        exit(EXIT_FAILURE);
    }

    printf("Semaphore ID = %d\n", semid);
    exit(EXIT_SUCCESS);
}
