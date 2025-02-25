#pragma once

#include "../include/TaskScheduler.hpp"
#include "levelClass.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class QueueRequests {
public:
    static QueueRequests* get();

    void addLevelToQueue(LevelCell* levelCell);
    void addLevelToTempQueue(LevelCell* levelCell);

    void removeLevelFromQueue(LevelCell* levelCell);
    void removeLevelFromTempQueue(LevelCell* levelCell);

    bool isQueuedInTempQueue(GJGameLevel* level);
    bool isQueuedInTempQueue(LevelCell* levelCell);

    bool isQueued(GJGameLevel* level);
    bool isQueued(LevelCell* levelCell);

    void clearQueue();
    void clearTempQueue();

    void startLoop();
private:
    bool isQueuedLevelsSuperiorToN(int n, bool isEqual);
    void saveInfosOfLevelsInArray(CCArray* levels);

    std::vector<LevelCell*> getLockedQueuedLevelList();
    std::vector<LevelCell*> getLockedTempQueuedLevelList();

    std::vector<WeakRef<LevelCell>> m_queuedLevelList;
    std::vector<WeakRef<LevelCell>> m_tempQueuedLevelList;
    TaskScheduler m_scheduler = TaskScheduler(1); // 1 thread
    int m_loopTaskID = 0;

    QueueRequests() = default;
    QueueRequests(const QueueRequests&) = delete;
    QueueRequests& operator=(const QueueRequests&) = delete;

    static QueueRequests* instance;
};