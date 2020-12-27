#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <errno.h>
#include <pthread.h>
typedef void *(*ROUTINE)(void *);

#define msg_output                                                         \
    printf("FUNC:%s LINE:%d failed,errno:%d,msg:%s\n", __func__, __LINE__, \
           errno, strerror(errno))

typedef struct work_queue {
    ROUTINE routine;  // 任务执行函数
    void *arg;        // 参数
    struct work_queue *next;
} work_queue_t;

typedef struct thread_pool {
    int shutdown;
    int max_thr_num;
    pthread_t *thr_id;
    work_queue_t *queue_head;
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_ready;
} thread_pool_t;

// 创建线程池
int thread_pool_create(int max_thr_num);

// 销毁线程池
void thread_poll_destroy();

// 像线程池中添加任务
int thread_poll_add_work(ROUTINE routine, void *arg);
#endif  // _THREAD_POOL_H_
