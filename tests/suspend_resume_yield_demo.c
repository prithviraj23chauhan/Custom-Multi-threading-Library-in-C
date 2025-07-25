#include "../Mythreads.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile int shared = 0;
int tidA, tidB;

void threadA() {
    printf("Thread A: started, will increment shared and suspend B.\n");
    for (int i = 0; i < 3; ++i) {
        shared++;
        printf("Thread A: shared = %d\n", shared);
        if (i == 1) {
            printf("Thread A: suspending Thread B (tid=%d)\n", tidB);
            suspend(tidB);
        }
        yield();
    }
    printf("Thread A: resuming Thread B (tid=%d)\n", tidB);
    resume(tidB);
    yield();
}

void threadB() {
    printf("Thread B: started, will increment shared.\n");
    for (int i = 0; i < 3; ++i) {
        shared += 2;
        printf("Thread B: shared = %d\n", shared);
        yield();
    }
}

int main() {
    tidA = create(threadA);
    tidB = create(threadB);
    start();
    clean();
    return 0;
}
