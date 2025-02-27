#pragma once

#include "../include/TaskScheduler.hpp"
#include "../level/levelClass.hpp"

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

    std::vector<WeakRef<LevelCell>> getQueue();
    std::vector<WeakRef<LevelCell>> getTempQueue();

    void startLoop();
    void stopLoop();
private:
    bool isQueuedLevelsSuperiorToN(int n, bool isEqual);
    void saveInfosOfLevelsInArray(CCArray* levels);

    std::vector<LevelCell*> getLockedQueuedLevelList();
    std::vector<LevelCell*> getLockedTempQueuedLevelList();

    //std::vector<WeakRef<LevelCell>> m_queuedLevelList;
    //std::vector<WeakRef<LevelCell>> m_tempQueuedLevelList;
    
    std::map<int, WeakRef<LevelCell>> m_queuedLevelList;
    std::map<int, WeakRef<LevelCell>> m_tempQueuedLevelList;

    TaskScheduler m_scheduler = TaskScheduler(1); // 1 thread
    int m_loopTaskID = 0;

    // Singleton thingy :
    QueueRequests() = default;
    QueueRequests(const QueueRequests&) = delete;
    QueueRequests& operator=(const QueueRequests&) = delete;

    static QueueRequests* instance;
};