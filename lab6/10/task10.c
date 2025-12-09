#include <pthread.h>
#include <stdio.h>

int data[10] = {1,2,3,4,5,6,7,8,9,10};

typedef struct {
    int start;
    int end; 
} Range;

void *sum_range(void *arg) {
    Range *r = (Range *)arg;
    int *res = malloc(sizeof(int));
    *res = 0;
    for (int i = r->start; i < r->end; i++) {
        *res += data[i];
    }
    pthread_exit(res);
}

int main(void) {
    pthread_t t1, t2;
    Range r1 = {0, 5};  // data[0..4]
    Range r2 = {5, 10}; // data[5..9]

    void *ret1, *ret2;

    pthread_create(&t1, NULL, sum_range, &r1);
    pthread_create(&t2, NULL, sum_range, &r2);

    pthread_join(t1, &ret1);
    pthread_join(t2, &ret2);

    int sum1 = *(int *)ret1;
    int sum2 = *(int *)ret2;

    free(ret1);
    free(ret2);

    int total = sum1 + sum2;
    printf("First half sum = %d\n", sum1);
    printf("Second half sum = %d\n", sum2);
    printf("Total sum = %d\n", total);

    return 0;
}
