#ifndef  THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <iostream>
#include "ThreadPool.h"

class ThreadPool
{
    std::vector<std::thread> workers;

    int max_workers;
    std::mutex mut;
    std::condition_variable cv;
    std::queue< std::function<void()> > tasks;
    bool stop = false;
    public:
    ThreadPool(int max_workers=1);
    ~ThreadPool();
    private:
    void workerLoop();
    public:
    bool add_task(std::function<void()> task);
    bool stop_all();


};


#endif