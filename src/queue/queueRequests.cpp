#include "./queueRequests.hpp"
#include "../level/levelInfos.hpp"
#include "../search/gdLevelSearch.hpp"
#include "../utils/utils.hpp"

QueueRequests* QueueRequests::instance = nullptr;

/*
What is the use of the temp queue ?
Uhhh idk here's a pseudo thing:

LevelCell::loadCustomLevel(GJGameLevel* level) {
  // show sprites
  addLevelToQueue(level);
}

queueTimer() {
  AddLevelToTempQueue(level);

  DoRequest(level);
  If (level->friendOnly()) {
    saveFriendOnlyLevel(level);
    removeFromTempQueue(level);
  }
}

TempQueue is because if the level hasn't been checked yet, and you close the game, you will still be able to check if the level is friend only
If instead I was only relying on the queue, if the level was queued but while the mod is checking if the level is friend only, you won't be know if it was really friend only
at the end of day, because the request didn't finish
*/

QueueRequests* QueueRequests::get() {
    static QueueRequests instance;
    return &instance;
}

void QueueRequests::addLevelToQueue(LevelCell* levelCell) {
    // log::debug("LevelInfos::wasAlreadyQueued(levelCell->m_level) = {}", std::to_string(LevelInfos::wasAlreadyQueued(levelCell->m_level)));
    // log::debug("isQueued(levelCell->m_level) = {}", std::to_string(isQueued(levelCell->m_level)));

    if (levelCell->m_level && !(LevelInfos::wasAlreadyQueued(levelCell->m_level))) {
        m_queuedLevelList[levelCell->m_level->m_levelID] = WeakRef(levelCell); // benefit of this:
    } else {                                                                   // This will replace the old levelCell to a new one, which we will be able to apply a given CCAction fade
        Misc::log_debug(fmt::format("Level {} won't be added to queue, as it was already in queue", levelCell->m_level->m_levelName));
    }
}

void QueueRequests::addLevelToTempQueue(LevelCell* levelCell) {
    // log::debug("LevelInfos::wasAlreadyQueued(levelCell->m_level) = {}", std::to_string(LevelInfos::wasAlreadyQueued(levelCell->m_level)));
    // log::debug("isQueued(levelCell->m_level) = {}", std::to_string(isQueued(levelCell->m_level)));

    if (levelCell->m_level) {
        m_tempQueuedLevelList[levelCell->m_level->m_levelID] = WeakRef(levelCell); // benefit of this:
    }                                                                              // This will replace the old levelCell to a new one, which we will be able to apply a given CCAction fade
}

void QueueRequests::removeLevelFromQueue(LevelCell* levelCell) {
    m_queuedLevelList.erase(levelCell->m_level->m_levelID);
}

void QueueRequests::removeLevelFromTempQueue(LevelCell* levelCell) {
    m_tempQueuedLevelList.erase(levelCell->m_level->m_levelID);
}

void QueueRequests::removeLevelFromTempQueue(int levelID) {
    m_tempQueuedLevelList.erase(levelID);
}

bool QueueRequests::isQueuedInTempQueue(GJGameLevel* level) {
    return m_tempQueuedLevelList.contains(level->m_levelID);
}

bool QueueRequests::isQueuedInTempQueue(LevelCell* levelCell) {
    return m_tempQueuedLevelList.contains(levelCell->m_level->m_levelID);
}

bool QueueRequests::isQueued(GJGameLevel* level) {
    return m_queuedLevelList.contains(level->m_levelID);
}

bool QueueRequests::isQueued(LevelCell* levelCell) {
    return m_queuedLevelList.contains(levelCell->m_level->m_levelID);
}

void QueueRequests::startLoop() {
    if ((!m_loopTaskID == 0) || (!Mod::get()->getSettingValue<bool>("let-queue-start"))) {
        return;
    }
    
    auto loopInterval = Mod::get()->getSettingValue<double>("queue-requests-timing");
    auto recurringTask = new ULILCTaskScheduler::Task(
        [this]() { // add 'this' in the capture list
            if (Mod::get()->getSettingValue<bool>("queue-requests")) { // also putting this in case the user turn off this feature while the loop is running
                // log::debug("isQueuedLevelsSuperiorToN() = {}", std::to_string(isQueuedLevelsSuperiorToN(0, false)));
                Misc::log_debug(fmt::format("queuedLevelList.size() = {}", std::to_string(getQueue().size())));

                if (m_queuedLevelList.size() > 0) {                   
                    LevelCell* levelCell = getLockedQueuedLevelList().at(0); // always get the first element
                    int levelID = getKeysFromQueuedLevels().at(0);
                    
                    if (levelCell) {
                        levelCell->retain();
                        addLevelToTempQueue(levelCell);
                    }
                    
                    if (!levelCell) {
                        log::error("LevelCell is nullptr !!!!!!! :(");
                    }
                    LevelSearch levelSearch;

                    GJGameLevel* level = levelCell->m_level;
                    log::info("Queued {}", level->m_levelName);

                    GJSearchObject* searchObject = GJSearchObject::create(SearchType::Search, std::to_string(levelID));
                    levelSearch.getGJLevels21(searchObject); // search for the level id

                    m_queuedLevelList.erase(levelID); // remove the first element in the vector
                    if (!m_queuedLevelList.contains(levelID)) {
                        if (levelCell) {
                            log::info("Removed {} from queue", level->m_levelName);
                        } else {
                            log::info("Removed level with ID {} from queue", levelID);
                        }
                    }
                }
            }
        },
        std::chrono::milliseconds(static_cast<int64_t>(loopInterval * 1000)), // The interval is defined by the user
        1
    );

    m_loopTaskID = recurringTask->getID();
    recurringTask->run();

    m_scheduler.addTask(recurringTask);
}

void QueueRequests::stopLoop() {
    m_scheduler.getTask(m_loopTaskID)->stop();
    m_loopTaskID = 0;
}

std::vector<LevelCell*> QueueRequests::getLockedQueuedLevelList() {
    std::vector<LevelCell*> values;

    auto queue = getQueue();

    for (size_t i = 0; i < queue.size(); i++)
    {
        values.push_back(queue.at(i).lock());
    }
    
    return values;
}

std::vector<int> QueueRequests::getKeysFromQueuedLevels() {
    std::vector<int> keys;

    for (auto [k, v] : m_queuedLevelList) { keys.push_back(k); }
    
    return keys;
}


std::vector<LevelCell*> QueueRequests::getLockedTempQueuedLevelList() {
    std::vector<LevelCell*> values;

    auto queue = getTempQueue();

    for (size_t i = 0; i < queue.size(); i++)
    {
        values.push_back(queue.at(i).lock());
    }
    
    return values;
}

std::vector<int> QueueRequests::getKeysFromTempQueuedLevels() {
    std::vector<int> keys;

    for (auto [k, v] : m_tempQueuedLevelList) { keys.push_back(k); }
    
    return keys;
}

void QueueRequests::clearQueue() {
    m_queuedLevelList.clear();
}

void QueueRequests::clearTempQueue() {
    m_tempQueuedLevelList.clear();
}

std::vector<WeakRef<LevelCell>> QueueRequests::getQueue() {
    return Misc::getValuesFromMap<int, WeakRef<LevelCell>>(m_queuedLevelList);
}

std::vector<WeakRef<LevelCell>> QueueRequests::getTempQueue() {
    return Misc::getValuesFromMap<int, WeakRef<LevelCell>>(m_tempQueuedLevelList);
}

void print_map(std::map<int, WeakRef<LevelCell>> const &m) {
    log::info("------------");
    for (auto const &pair: m) {
        if (pair.second.valid()) {
            log::info("({}: VALID LEVEL CELL)", std::to_string(pair.first));
        } else {
            log::info("({}: NOT A VALID LEVEL CELL)", std::to_string(pair.first));
        }
    }
    log::info("------------");
}

WeakRef<LevelCell> QueueRequests::getLevelCellFromLevelID(int levelID, bool isTempQueue) {
    std::map<int, WeakRef<LevelCell>> queue;

    if (isTempQueue) {
        queue = m_tempQueuedLevelList;
    } else {
        queue = m_queuedLevelList;
    }
    
    if (Mod::get()->getSettingValue<bool>("show-debug-logs")) {
        print_map(queue);
    }
    return queue[levelID];
}