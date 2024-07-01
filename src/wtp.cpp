#include "wtp.h"

wtp::wtp(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workerIds.push_back(i);
        workers.emplace_back(&wtp::worker, this, i);
    }
}

wtp::~wtp() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
}

void wtp::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.push(task);
    }
    condition.notify_one();
}

void wtp::worker(size_t id) {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });
            if (stop && tasks.empty())
                return;
            task = std::move(tasks.front());
            tasks.pop();
        }
        std::cout << "[WTP] - Executing worker ID " << id << std::endl;
        task();
    }
}