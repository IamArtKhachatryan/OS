#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *double_func(void *arg) {
    int x = *(int *)arg;
    int *res = malloc(sizeof(int));
    if (!res) pthread_exit(NULL);
    *res = 2 * x;
    pthread_exit(res);
}

void *triple_func(void *arg) {
    int x = *(int *)arg;
    int *res = malloc(sizeof(int));
    if (!res) pthread_exit(NULL);
    *res = 3 * x;
    pthread_exit(res);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s number\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    pthread_t t1, t2;
    void *ret1, *ret2;

    pthread_create(&t1, NULL, double_func, &n);
    pthread_join(t1, &ret1);

    int doubled = *(int *)ret1;
    free(ret1);

    pthread_create(&t2, NULL, triple_func, &doubled);
    pthread_join(t2, &ret2);

    int tripled = *(int *)ret2;
    free(ret2);

    printf("Result = %d\n", tripled);
    return 0;
}
