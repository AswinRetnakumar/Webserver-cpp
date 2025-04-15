#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <iostream>
#include "ThreadPool.h"


ThreadPool::ThreadPool(int max_workers)
{
    max_workers = max_workers;
    for(int w=0; w<max_workers; w++)
    {
        // using emplace_back rather than push back to skip creating a
        // thread using std::thread
        // this is used in capture class  as workerLoop is class member..
        workers.emplace_back([this](){ workerLoop();});
    }
    std::cout << "Thread pool created with " << max_workers << " workers" << std::endl;
}

ThreadPool::~ThreadPool()
{
    ThreadPool::stop_all();
}

void ThreadPool::workerLoop()
{   
    std::function<void()> task;
    while(true)
    {
        std::unique_lock<std::mutex> lock(mut);
        cv.wait(lock, [this]{
            return stop || !tasks.empty();
        });
        if(stop && tasks.empty())
        {
            break;
        }

        task = move(tasks.front());
        tasks.pop();
        lock.unlock();
        try
        {
            task();
        }
        catch(...)
        {
            std::cout << "Failure to execute tasks" << std::endl;
        }
        
    }
}

bool ThreadPool::add_task(std::function<void()> task)
{
    try
    {
        std::unique_lock<std::mutex> lock(mut);
        if(!stop)
        {
            tasks.emplace(task);
            cv.notify_one();
        }
        else 
        {
            std::cout << "Thread pool is stopped. Cannot add new tasks" << std::endl;
            return false;
        }
        
        lock.unlock();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    
    return true;
    
}

bool ThreadPool::stop_all()
{
    try
    {
        std::unique_lock<std::mutex> lock(mut);
        stop = true;
        cv.notify_all();
        lock.unlock();
        std::cout << "Notified all threads for stoppping"  << std::endl;
        for(auto &worker: workers)
        {
            worker.join();
        }

        return true;
    }
    
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    
}



