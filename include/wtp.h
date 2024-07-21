// Copyright (c) 2024 WryKun

#ifndef WTP_H // WryKun Threadpool Process
#define WTP_H // WryKun Threadpool Process

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>

class wtp {
public:
    wtp(size_t numThreads);
    ~wtp();
    void enqueue(std::function<void()> task);

private:
    std::vector<std::thread> workers;
    std::vector<size_t> workerIds;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;

    void worker(size_t id);
};

#endif
