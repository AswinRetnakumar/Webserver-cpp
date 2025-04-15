#include "ThreadPool.h"
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>

// Test: Add tasks to the thread pool and ensure they are executed
TEST(ThreadPoolTest, AddTaskAndExecute) {
    ThreadPool pool(4); // Create a thread pool with 4 workers

    std::atomic<int> counter = 0;

    // Add 10 tasks to the thread pool
    for (int i = 0; i < 10; ++i) {
        pool.add_task([&counter]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Simulate work
            counter++;
        });
    }

    // Allow some time for tasks to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Stop the thread pool
    pool.stop_all();

    // Verify that all tasks were executed
    EXPECT_EQ(counter, 10);
}

// Test: Stop the thread pool gracefully
TEST(ThreadPoolTest, StopAll) {
    ThreadPool pool(4); // Create a thread pool with 4 workers

    // Add a task that takes some time
    pool.add_task([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });

    // Stop the thread pool immediately
    pool.stop_all();

    // Verify that the thread pool stopped without crashing
    SUCCEED();
}

// Test: Ensure no tasks are executed after stopping the thread pool
TEST(ThreadPoolTest, NoTasksAfterStop) {
    ThreadPool pool(4); // Create a thread pool with 4 workers

    std::atomic<int> counter = 0;

    // Add a task to increment the counter
    pool.add_task([&counter]() {
        counter++;
    });

    // Stop the thread pool
    pool.stop_all();

    // Try adding another task after stopping
    bool result = pool.add_task([&counter]() {
        counter++;
    });

    // Verify that the task was not added
    EXPECT_FALSE(result);

    // Verify that the counter was incremented only once
    EXPECT_EQ(counter, 1);
}