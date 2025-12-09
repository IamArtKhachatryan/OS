#include <pthread.h>
#include <stdio.h>

int data[10];

void *fill_even_indexes(void *arg) {
    for (int i = 0; i < 10; i += 2) {
        data[i] = i + 1;
    }
    return NULL;
}

int main(void) {
    pthread_t th;

    pthread_create(&th, NULL, fill_even_indexes, NULL);

    for (int i = 1; i < 10; i += 2) {
        data[i] = i + 1;
    }

    pthread_join(th, NULL);

    for (int i = 0; i < 10; i++) {
        printf("data[%d] = %d\n", i, data[i]);
    }

    return 0;
}
