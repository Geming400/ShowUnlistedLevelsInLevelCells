#pragma once

#include <thread>
#include <vector>
#include <functional>
#include <chrono>
#include <atomic>

namespace ULILCTaskScheduler { // ULILC = Unlisted Levels In Level Cells
    /*
    @note This is a task, which means it will repeat every x milliseconds until stopped

    ```cpp
    #include "newTaskScheduler.hpp"
    #include <thread>
    #include <iostream>

    using namespace ULILCTaskScheduler;

    Task myTask = Task([]() { std::cout << "Hi" << "\n"; }, std::chrono::seconds(1));
    myTask.run();

    std::this_thread::sleep_for(std::chrono::seconds(10))

    myTask.stop();
    ```

    */
    class Task {
    public:
        Task(std::function<void()> func, std::chrono::milliseconds interval, unsigned int taskID);
        ~Task();

        void run();
        void stop(bool stop_thread = true);

        unsigned int getID() const;
    private:
        void execute();

        std::atomic<bool> m_isRunning = false;
        std::function<void()> m_func;
        unsigned int m_taskID;

        std::thread m_thread;
        std::chrono::milliseconds m_interval;
    };



    class NewTaskScheduler
    {
    public:
        ~NewTaskScheduler();

        void addTask(Task task);
        void addTask(Task* task);
        void removeTask(unsigned int const taskID);

        void runAllTasks();
        void stopAllTasks();

        Task* getTask(unsigned int const taskID);

        Task* operator[](size_t const id);
    private:
        std::vector<Task*> m_tasks;
        std::map<int, int> m_tasksIndexexToID;
    };
}