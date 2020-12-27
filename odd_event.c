#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mtx;
pthread_cond_t cond1;
pthread_cond_t cond2;
int g_num = 1;
void *print_odd(void *arg) {
    while (1) {
        sleep(1);
        pthread_mutex_lock(&mtx);
        printf("Thread:1 g_num=%d\n", g_num);
        g_num++;
        pthread_cond_signal(&cond2);
        pthread_cond_wait(&cond1, &mtx);
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

void *print_even(void *arg) {
    while (1) {
        sleep(1);
        pthread_mutex_lock(&mtx);
        printf("Thread:2 g_num=%d\n", g_num);
        g_num++;
        pthread_cond_signal(&cond1);
        pthread_cond_wait(&cond2, &mtx);
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

int main(int argc, char const *argv[]) {
    pthread_t p1;
    pthread_t p2;
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&cond1, NULL);
    pthread_cond_init(&cond2, NULL);
    pthread_create(&p1, NULL, print_odd, NULL);
    pthread_create(&p2, NULL, print_even, NULL);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);

    return 0;
}
