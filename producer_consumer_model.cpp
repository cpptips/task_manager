#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

// 生产者消费者基本模型

std::mutex mtx;              // 一个互斥锁
std::condition_variable cv;  // 一个条件变量
bool ready = false;

// 生产者
void producer(int i) {
    std::unique_lock<std::mutex> lock(mtx);
    while (!ready) cv.wait(lock);
    std::cout << i << std::endl;
}

// 消费者
void consumer() {
    std::unique_lock<std::mutex> lock(mtx);
    ready = true;
    cv.notify_all();
}

int main() {
    std::thread th[10];
    for (int i = 0; i < 10; i++) {
        th[i] = std::thread(producer, i);
    }
    std::cout << "start print" << std::endl;

    consumer();
    for (auto& t : th) {
        t.join();
    }
    return 0;
}