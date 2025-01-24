#pragma once

#include "../include/TaskScheduler.hpp"
#include "levelClass.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class QueueRequests {
public:
    void addLevelToQueue(LevelClass levelClass);
    void addLevelToTempQueue(LevelClass levelClass);

    void removeLevelFromQueue(LevelClass levelClass);
    void removeLevelFromTempQueue(LevelClass levelClass);

    bool isQueuedInTempQueue(GJGameLevel* level);
    bool isQueuedInTempQueue(LevelClass levelClass);

    bool isQueued(GJGameLevel* level);
    bool isQueued(LevelClass levelClass);

    void startLoop();
private:
    bool isQueuedLevelsSuperiorToN(int n, bool isEqual);
    void saveInfosOfLevelsInArray(CCArray* levels);

    std::vector<LevelClass> m_queuedLevelList;
    std::vector<LevelClass> m_tempQueuedLevelList;
    TaskScheduler m_scheduler = TaskScheduler(1); // 1 thread
    int m_loopTaskID;
};