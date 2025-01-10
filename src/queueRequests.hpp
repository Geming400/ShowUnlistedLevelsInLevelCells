#pragma once

#include "../include/TaskScheduler.hpp"
#include "levelClass.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class QueueRequests {
public:
    void addLevelToQueue(LevelClass levelClass);
    void addLevelToTempQueue(LevelClass levelClass);
    bool isQueued(LevelClass levelClass, bool isFromTempQueue);
    bool isQueued(GJGameLevel* level);
    void startLoop();
private:
    bool isQueuedLevelsSuperiorToN(int n, bool isEqual);
    void saveInfosOfLevelsInArray(CCArray* levels);

    std::vector<LevelClass> m_queuedLevelList;
    std::vector<LevelClass> m_tempQueuedLevelList;
    TaskScheduler m_scheduler;
    int m_loopTaskID;
};