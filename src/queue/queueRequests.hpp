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
    void removeLevelFromTempQueue(LevelCell* levelCell);
    void removeLevelFromTempQueue(int levelID);

    bool isQueuedInTempQueue(GJGameLevel* level);
    bool isQueuedInTempQueue(LevelCell* levelCell);

    bool isQueued(GJGameLevel* level);
    bool isQueued(LevelCell* levelCell);

    void clearQueue();
    void clearTempQueue();

    WeakRef<LevelCell> getLevelCellFromLevelID(int levelID, bool isTempQueue = false);

    std::vector<WeakRef<LevelCell>> getQueue();
    std::vector<WeakRef<LevelCell>> getTempQueue();

    void startLoop();
    void stopLoop();
    void onGameQuit();
private:
    void saveInfosOfLevelsInArray(CCArray* levels);

    std::vector<LevelCell*> getLockedQueuedLevelList();
    std::vector<int> getKeysFromQueuedLevels();

    std::vector<LevelCell*> getLockedTempQueuedLevelList();
    std::vector<int> getKeysFromTempQueuedLevels();

    //std::vector<WeakRef<LevelCell>> m_queuedLevelList;
    //std::vector<WeakRef<LevelCell>> m_tempQueuedLevelList;
    
    std::map<int, WeakRef<LevelCell>> m_queuedLevelList;
    std::map<int, WeakRef<LevelCell>> m_tempQueuedLevelList;

    // TaskScheduler m_scheduler = TaskScheduler(1); // 1 thread
    ULILCTaskScheduler::NewTaskScheduler m_scheduler;
    int m_loopTaskID = 0;

    // Singleton thingy :
    QueueRequests() = default;
    QueueRequests(const QueueRequests&) = delete;
    QueueRequests& operator=(const QueueRequests&) = delete;

    static QueueRequests* instance;
};