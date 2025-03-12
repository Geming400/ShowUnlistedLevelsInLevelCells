#include "NewTaskScheduler.hpp"

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

void Task::stop() {
    m_isRunning = false;
    if (m_thread.joinable()) {
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



NewTaskScheduler::~NewTaskScheduler() {
    for (auto task : m_tasks)
    {
        delete task;
    }
}

void NewTaskScheduler::addTask(Task task) {
    m_tasksIndexexToID[task.getID()] = m_tasks.size();
    m_tasks.push_back(&task);
}

void NewTaskScheduler::addTask(Task* task) {
    m_tasksIndexexToID[task->getID()] = m_tasks.size();
    m_tasks.push_back(task);
}


void NewTaskScheduler::removeTask(const unsigned int taskID) {
    Task* task = m_tasks[m_tasksIndexexToID[taskID]];
    task->stop();

    m_tasks.erase(
        m_tasks.begin() + m_tasksIndexexToID[taskID]
    );

    m_tasksIndexexToID.erase(taskID);
}

Task* NewTaskScheduler::getTask(const unsigned int taskID) {
    for (auto task : m_tasks)
    {
        if (task->getID() == taskID) {
            return task;
        }
    }
    return nullptr;
}