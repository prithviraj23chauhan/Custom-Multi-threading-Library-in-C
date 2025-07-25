#include "../Mythreads.h"
#include <stdio.h>
#include <stdlib.h>

void hello_thread() {
    printf("Hello from a thread with no arguments!\n");
}

void* sum_thread(void* arg) {
    int n = *(int*)arg;
    int sum = 0;
    for (int i = 1; i <= n; ++i) sum += i;
    printf("Sum from 1 to %d is %d\n", n, sum);
    int* result = (int*)malloc(sizeof(int));
    *result = sum;
    return result;
}

int main() {
    int tid1 = create(hello_thread);
    int n = 10;
    int tid2 = createWithArgs(sum_thread, &n);

    start();

    join(tid2);
    int* result = (int*)GetThreadResult(tid2);
    if (result) {
        printf("Result from sum_thread: %d\n", *result);
        free(result);
    }
    clean();
    return 0;
}
