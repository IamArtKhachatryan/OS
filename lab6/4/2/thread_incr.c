// task4_seq_increment.c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int glob = 0;
int loops;

void *threadFunc(void *arg) {
    for (int i = 0; i < loops; i++) {
        glob++;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t t1, t2;

    loops = (argc > 1) ? atoi(argv[1]) : 10000000;

    if (pthread_create(&t1, NULL, threadFunc, NULL) != 0) {
        perror("pthread_create t1");
        return 1;
    }
    pthread_join(t1, NULL);

    if (pthread_create(&t2, NULL, threadFunc, NULL) != 0) {
        perror("pthread_create t2");
        return 1;
    }
    pthread_join(t2, NULL);

    printf("glob = %d (expected %d)\n", glob, 2 * loops);
    return 0;
}
