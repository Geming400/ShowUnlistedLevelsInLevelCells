#include "NewTaskScheduler.hpp"
#include <utility>

using namespace ULILCTaskScheduler;

Task::Task(std::function<void()> func, std::chrono::milliseconds interval, unsigned int taskID) {
    m_func = func;
    m_interval = interval;
    m_taskID = taskID;
}

Task::~Task() {
    stop();
}

void Task::run() {
    if (!m_isRunning) {
        m_isRunning = true;
        m_thread = std::thread(&Task::execute, this);
    }
}

void Task::stop(bool stop_thread) {
    m_isRunning = false;
    if (m_thread.joinable() && stop_thread) {
        m_thread.join();
    }
}

void Task::execute() {
    while (m_isRunning) {
        try {
            m_func();
            std::this_thread::sleep_for(m_interval);
        } catch (...) {
            // Ignore all exceptions
        }
    }
}

unsigned int Task::getID() const {
    return m_taskID;
}


NewTaskScheduler::NewTaskScheduler(size_t maxTaskCount) {
    m_maxTaskCount = maxTaskCount;
}

NewTaskScheduler::~NewTaskScheduler() {
    for (auto task : m_tasks)
    {
        if (task) {
            delete task;
        }
    }
}

void NewTaskScheduler::addTask(Task task) {
    addTask(&task);
}

void NewTaskScheduler::addTask(Task* task) {
    auto actuallyAddTask = [this, task]() {
        if (task) {
            m_tasksIndexexToID[task->getID()] = m_tasks.size() - 1;
            m_tasks.push_back(task);
        }
    };

    if (m_maxTaskCount == -1) {
        actuallyAddTask();
    } else {
        if (m_maxTaskCount >= m_tasks.size()) {
            actuallyAddTask();
        }
    }
}

void NewTaskScheduler::removeTask(unsigned const int taskID) {
    Task* task = m_tasks[m_tasksIndexexToID[taskID]];
    task->stop();

    m_tasks.erase(
        m_tasks.begin() + m_tasksIndexexToID[taskID]
    );

    m_tasksIndexexToID.erase(taskID);
}

Task* NewTaskScheduler::getTask(unsigned const int taskID) {
    for (auto task : m_tasks)
    {
        if (task->getID() == taskID) {
            return task;
        }
    }
    return nullptr;
}

void NewTaskScheduler::runAllTasks() {
    for (auto task : m_tasks)
    {
        task->run();
    }
}

void NewTaskScheduler::stopAllTasks() {
    for (auto task : m_tasks)
    {
        task->stop(false);
    }
}

Task* NewTaskScheduler::operator[](const size_t id) {
    return m_tasks[m_tasksIndexexToID[id]];
}