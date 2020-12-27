#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread_pool.h"

void *process(void *arg) {
    printf("thread %d\n", (int)arg);
    return NULL;
}

int main(int argc, char **argv) {
    if (thread_pool_create(7) != 0) {
        msg_output;
        exit(1);
    }

    for (size_t i = 0; i < 1300; ++i) {
        thread_poll_add_work(process, (void *)i);
    }

    sleep(7);
    thread_pool_destroy();
    return 0;
}