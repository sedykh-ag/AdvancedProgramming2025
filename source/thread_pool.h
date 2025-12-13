#pragma once

#include <condition_variable>
#include <format>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "optick.h"


class ThreadPool {
public:
    ThreadPool(size_t num_threads
               = std::thread::hardware_concurrency())
    {
        for (size_t i = 0; i < num_threads; ++i) {
            threads_.emplace_back([this, i] {
                OPTICK_THREAD(std::format("worker {}", i).c_str());
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(
                            queue_mutex_);

                        cv_.wait(lock, [this] {
                            return !tasks_.empty() || stop_;
                        });

                        if (stop_ && tasks_.empty()) {
                            return;
                        }

                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }

                    task();

                    {
                        std::unique_lock<std::mutex> lock(queue_mutex_);
                        --unfinished_tasks_;
                        if (unfinished_tasks_ == 0)
                            finished_all_tasks_cv_.notify_one();
                    }
                }
            });
        }
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            stop_ = true;
        }
        cv_.notify_all();
        for (auto& thread : threads_) {
            thread.join();
        }
    }

    void enqueue(std::function<void()> task)
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            tasks_.emplace(std::move(task));
            ++unfinished_tasks_;
        }
        cv_.notify_one();
    }

    void wait_all_tasks_done()
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        finished_all_tasks_cv_.wait(lock, [this] {
            return unfinished_tasks_ == 0;
        });
    }

private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    std::condition_variable finished_all_tasks_cv_;
    bool stop_ = false;
    size_t unfinished_tasks_ = 0;
};
