// 多线程同步打印奇数偶数
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx;
std::condition_variable cv1;
std::condition_variable cv2;
int g_num = 1;
void print_odd() {
    while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::unique_lock<std::mutex> lock(mtx);
        std::cout << "Thread 1, g_num =" << g_num++ << std::endl;
        cv2.notify_one();
        cv1.wait(lock);
    }
}

void print_even() {
    while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::unique_lock<std::mutex> lock(mtx);
        std::cout << "Thread 2, g_num =" << g_num++ << std::endl;
        cv1.notify_one();
        cv2.wait(lock);
    }
}

int main(int argc, char const *argv[]) {
    std::thread t1(print_odd);
    std::thread t2(print_even);
    t1.join();
    t2.join();
    return 0;
}
