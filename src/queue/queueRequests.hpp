#pragma once

//#include "../include/TaskScheduler.hpp"
#include "../include/NewTaskScheduler.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class QueueRequests {
public:
    static QueueRequests* get();

    void addLevelToQueue(LevelCell* levelCell);
    void addLevelToTempQueue(LevelCell* levelCell);

    void removeLevelFromQueue(LevelCell* levelCell);

    bool isQueuedInTempQueue(GJGameLevel* level);
    bool isQueuedInTempQueue(LevelCell* levelCell);

    bool isQueued(GJGameLevel* level);
    bool isQueued(LevelCell* levelCell);

    void clearQueue();
    void clearTempQueue();

    WeakRef<LevelCell> getLevelCellFromLevelID(int levelID);
    WeakRef<LevelCell> getStoredTempStoredLevel();

    std::vector<WeakRef<LevelCell>> getQueue();
    std::vector<WeakRef<LevelCell>> getTempQueue();

    void startLoop();
    void stopLoop();
    void onGameQuit();
private:
    std::vector<LevelCell*> getLockedQueuedLevelList();
    std::vector<int> getKeysFromQueuedLevels();

    //std::vector<WeakRef<LevelCell>> m_queuedLevelList;
    //std::vector<WeakRef<LevelCell>> m_tempQueuedLevelList;
    
    std::map<int, WeakRef<LevelCell>> m_queuedLevelList;
    //std::map<int, WeakRef<LevelCell>> m_tempQueuedLevelList;
    WeakRef<LevelCell> m_tempStoredLevelCell;

    // TaskScheduler m_scheduler = TaskScheduler(1); // 1 thread
    ULILCTaskScheduler::NewTaskScheduler m_scheduler = ULILCTaskScheduler::NewTaskScheduler(1);
    int m_loopTaskID = 0;

    // Singleton thingy :
    QueueRequests() = default;
    QueueRequests(const QueueRequests&) = delete;
    QueueRequests& operator=(const QueueRequests&) = delete;

    static QueueRequests* instance;
};