#include "workers.hpp"

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include <functional>
#include <memory>

std::mutex mtx;
std::condition_variable cv;
bool stop = false;

using Task = std::function<void()>;
std::queue<Task> task_queue;


void thread_callback(std::stop_token stoken)
{
    while (!stoken.stop_requested()) {
        Task task;
        {
            std::unique_lock lock(mtx);
            cv.wait(lock, [] { return stop || !task_queue.empty(); });

            if (stop && task_queue.empty()) break;

            task = std::move(task_queue.front());
            task_queue.pop();
        }
        task();  // run task
    }

}

std::vector<std::jthread> worker_threads;

void start_workers()
{
    std::cout << "Starting workers..." << std::endl;
    worker_threads.emplace_back(std::move(thread_callback));
    worker_threads.emplace_back(std::move(thread_callback));
}

void request_command(Task &&t)
{
    {
        std::lock_guard lock(mtx);
        task_queue.push(std::move(t));
    }
    cv.notify_one();
}

