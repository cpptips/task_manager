#include "thread_pool.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static thread_pool_t *pool = NULL;

// 工作线程，从任务队列中取任务并执行
static void *thread_worker(void *arg) {
    work_queue_t *work = NULL;
    while (1) {
        pthread_mutex_lock(&pool->queue_lock);
        while (!pool->queue_head && !pool->shutdown) {
            //没有任务就阻塞再这个地方
            pthread_cond_wait(&pool->queue_ready, &pool->queue_lock);
        }

        if (pool->shutdown) {
            printf(" worker loseing\n");
            pthread_mutex_unlock(&pool->queue_lock);
            pthread_exit(NULL);
        }
        printf(" worker running\n");
        work = pool->queue_head;
        pool->queue_head = pool->queue_head->next;
        pthread_mutex_unlock(&pool->queue_lock);
        work->routine(work->arg);  //执行任务
        free(work);
    }
    return NULL;
}

// 创建线程池
int thread_pool_create(int max_thr_num) {
    pool = calloc(1, sizeof(thread_pool_t));
    if (!pool) {
        printf("FUNC:%s erorr\n", __func__);
        exit(1);
    }

    //初始化
    pool->max_thr_num = max_thr_num;
    pool->shutdown = 0;
    pool->queue_head = NULL;
    if (pthread_mutex_init(&pool->queue_lock, NULL) != 0) {
        printf("FUNC:%s queue_lock failed,errno:%d,msg:%s\n", __func__, errno,
               strerror(errno));
        exit(1);
    }
    if (pthread_cond_init(&pool->queue_ready, NULL) != 0) {
        printf("FUNC:%s queue_ready failed,errno:%d,msg:%s\n", __func__, errno,
               strerror(errno));
        exit(1);
    }

    // 创建工作线程
    pool->thr_id = calloc(max_thr_num, sizeof(pthread_t));
    if (!pool->thr_id) {
        msg_output;
        exit(1);
    }

    for (size_t i = 0; i < max_thr_num; i++) {
        if (pthread_create(&pool->thr_id[i], NULL, thread_worker, NULL) != 0) {
            msg_output;
            exit(1);
        }
    }
    printf("create success\n");
    return 0;
}

// 销毁线程池
void thread_pool_destroy() {
    if (pool->shutdown) {
        return;
    }

    //通知所有正在等待的工作线程
    pool->shutdown = 1;
    pthread_mutex_lock(&pool->queue_lock);
    pthread_cond_broadcast(&pool->queue_ready);
    pthread_mutex_unlock(&pool->queue_lock);

    // join工作线程
    for (size_t i = 0; i < pool->max_thr_num; i++) {
        pthread_join(pool->thr_id[i], NULL);
    }
    free(pool->thr_id);

    // 释放任务队列
    work_queue_t *member = NULL;
    while (pool->queue_head) {
        member = pool->queue_head;
        pool->queue_head = pool->queue_head->next;
        free(member);
        member = NULL;
        printf("free sucess\n");
    }

    pthread_mutex_destroy(&pool->queue_lock);
    pthread_cond_destroy(&pool->queue_ready);
    free(pool);
}

int thread_poll_add_work(ROUTINE routine, void *arg) {
    if (!routine) {
        msg_output;
        return -1;
    }

    work_queue_t *work = calloc(1, sizeof(work_queue_t));
    if (!work) {
        msg_output;
        return -1;
    }

    work->routine = routine;
    work->arg = arg;
    work->next = NULL;

    pthread_mutex_lock(&pool->queue_lock);
    work_queue_t *member = pool->queue_head;
    if (!member) {
        pool->queue_head = work;
    } else {
        while (member->next) {
            member = member->next;
        }
        member->next = work;  // ring
    }
    // 通知工作线程，有任务加入
    pthread_cond_signal(&pool->queue_ready);
    pthread_mutex_unlock(&pool->queue_lock);

    printf("add sucess\n");
    return 0;
}