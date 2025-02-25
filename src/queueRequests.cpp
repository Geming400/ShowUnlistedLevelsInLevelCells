#include "queueRequests.hpp"
#include "levelInfos.hpp"
#include "levelClass.hpp"
#include "gdLevelSearch.hpp"

#include "../include/TaskScheduler.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

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

// TempQueue is because if the level hasn't been checked yet, and you close the game, you will still be able to check if the level is friend only
// If instead I was only relying on the queue, if the level was queued but while the mod is checking if the level is friend only, you won't be know if it was really friend only
// at the end of day, because the request didn't finish
*/

bool hasUserToggledOnQueuedRequests() {
    return Mod::get()->getSettingValue<bool>("queue-requests");
}

QueueRequests* QueueRequests::get() {
    static QueueRequests instance;
    return &instance;
}

void QueueRequests::addLevelToQueue(LevelCell* levelCell) {
    // log::debug("LevelInfos::wasAlreadyQueued(levelCell->m_level) = {}", std::to_string(LevelInfos::wasAlreadyQueued(levelCell->m_level)));
    // log::debug("isQueued(levelCell->m_level) = {}", std::to_string(isQueued(levelCell->m_level)));
    
    levelCell->retain();

    if (levelCell->m_level && !(LevelInfos::wasAlreadyQueued(levelCell->m_level) || isQueued(levelCell->m_level))) {
        m_queuedLevelList.push_back(WeakRef(levelCell));
    } else {
        log::info("Level {} won't be added to queue, as it's already in queue or was already in queue", levelCell->m_level->m_levelName);
    }
}

void QueueRequests::addLevelToTempQueue(LevelCell* levelCell) {

    levelCell->retain();

    log::debug("QueueRequests::addLevelToTempQueue()");
    // log::debug("LevelInfos::wasAlreadyQueued(levelCell->m_level) = {}", std::to_string(LevelInfos::wasAlreadyQueued(levelCell->m_level)));
    // log::debug("isQueued(levelCell->m_level) = {}", std::to_string(isQueued(levelCell->m_level)));

    if (levelCell->m_level && !isQueuedInTempQueue(levelCell->m_level)) {
        m_tempQueuedLevelList.push_back(WeakRef(levelCell));
    } else {
        log::info("Level {} won't be added to the temp queue because the level is already in the temp queue", levelCell->m_level->m_levelName);
    }
}

void QueueRequests::removeLevelFromQueue(LevelCell* levelCell) {
    log::debug("QueueRequests::removeLevelFromQueue()");
    if (levelCell->m_level && (LevelInfos::wasAlreadyQueued(levelCell->m_level) || isQueued(levelCell->m_level))) {
        auto queuedLevelList = getLockedQueuedLevelList();

        auto pos = std::find(
                queuedLevelList.begin(), queuedLevelList.end(),
                levelCell
            );
        
        if (pos != queuedLevelList.end()) {
            size_t index = std::distance(queuedLevelList.begin(), pos);

            m_queuedLevelList.erase(m_queuedLevelList.begin() + index);
        }

        levelCell->release();
    } else {
        log::info("Level {} won't be removed in queue, as it wasn't already in queue or wasn't already in queue", levelCell->m_level->m_levelName);
    }
}

void QueueRequests::removeLevelFromTempQueue(LevelCell* levelCell) {
    log::debug("QueueRequests::removeLevelFromTempQueue()");
    if (levelCell->m_level && isQueuedInTempQueue(levelCell->m_level)) {
        auto tempQueuedLevelList = getLockedTempQueuedLevelList();
        log::debug("Before std::find -> m_tempQueuedLevelList size: {}", m_tempQueuedLevelList.size());
        //log::debug("removeLevelFromTempQueue -> Starting std::find with level: {}",
        //   levelCell->m_level ? levelCell->m_level->m_levelName : "nullptr");
        auto pos = std::find(
                tempQueuedLevelList.begin(), tempQueuedLevelList.end(),
                levelCell
            );
        
        if (pos != tempQueuedLevelList.end()) {
            size_t index = std::distance(tempQueuedLevelList.begin(), pos);

            m_tempQueuedLevelList.erase(m_tempQueuedLevelList.begin() + index);
        } else {
            for (size_t i = 0; i < m_tempQueuedLevelList.size(); i++)
            {
                auto level = m_tempQueuedLevelList.at(i).lock()->m_level;
                auto name = level->m_levelName;
            }
            
            log::debug("if (pos == m_tempQueuedLevelList.end())");
        }

        levelCell->release();
    } else {
        log::info("Level {} won't be removed in queue, as it wasn't already in the temp queue", levelCell->m_level->m_levelName);
    }
}


bool QueueRequests::isQueuedInTempQueue(GJGameLevel* level) {
    int levelID = level->m_levelID.value();

    for (size_t i = 0; i < m_tempQueuedLevelList.size(); i++)
	{   
        GJGameLevel* queuedLevel = m_tempQueuedLevelList.at(i).lock()->m_level;
        if (queuedLevel) {
            int queuedLevelID = queuedLevel->m_levelID.value();

            if (levelID == queuedLevelID) {
                return true;
            }
        }
	}
    
    return false;
}

bool QueueRequests::isQueuedInTempQueue(LevelCell* levelCell) {
    int levelID = levelCell->m_level->m_levelID.value();

    for (size_t i = 0; i < m_tempQueuedLevelList.size(); i++)
    {   
        if (auto obj = m_tempQueuedLevelList.at(i).lock()) {
            GJGameLevel* queuedLevel = obj->m_level;
        
            int queuedLevelID = queuedLevel->m_levelID.value();

            if (levelID == queuedLevelID) {
                return true;
            }
        }
    }
    
    return false;
}

bool QueueRequests::isQueued(GJGameLevel* level) {
    int levelID = level->m_levelID.value();

    for (size_t i = 0; i < m_queuedLevelList.size(); i++)
	{
        if (auto obj = m_queuedLevelList.at(i).lock()) {
            GJGameLevel* queuedLevel = obj->m_level;
            int queuedLevelID = queuedLevel->m_levelID.value();

            if (levelID == queuedLevelID) {
                return true;
            }
        }
	}
    
    return false;
}

bool QueueRequests::isQueued(LevelCell* levelCell) {
    int levelID = levelCell->m_level->m_levelID.value();

    for (size_t i = 0; i < m_queuedLevelList.size(); i++)
	{
        if (auto obj = m_queuedLevelList.at(i).lock()) {
            GJGameLevel* queuedLevel = obj->m_level;
            int queuedLevelID = queuedLevel->m_levelID.value();

            if (levelID == queuedLevelID) {
                return true;
            }
        }
	}
    
    return false;
}

void QueueRequests::startLoop() {
    if (!m_loopTaskID == 0) {
        return;
    }

    auto loopInterval = Mod::get()->getSettingValue<int64_t>("queue-requests-timing");
    auto recurringTask = std::make_unique<RecurringTask>(
        [this]() { // add 'this' in the capture list
            if (hasUserToggledOnQueuedRequests()) { // also putting this in case the user turn off this feature while the loop is running
                // log::debug("isQueuedLevelsSuperiorToN() = {}", std::to_string(isQueuedLevelsSuperiorToN(0, false)));
                log::debug("queuedLevelList.size() = {}", std::to_string(m_queuedLevelList.size()));

                if (isQueuedLevelsSuperiorToN(0, false)) {
                    // LMD* levelManagerDelegate = new LMD();
                    // GameLevelManager* gameLevelManager = GameLevelManager::sharedState();
                    // levelManagerDelegate->originalDelegate = gameLevelManager->m_levelManagerDelegate;

                    // gameLevelManager->m_levelManagerDelegate = levelManagerDelegate;
                    
                    LevelCell* levelCell = getLockedQueuedLevelList().at(0); // always get the first element
                    if (levelCell) {
                        addLevelToTempQueue(levelCell);
                    }

                    auto levelSearch = new LevelSearch(levelCell, this);

                    GJGameLevel* level = levelCell->m_level;
                    log::info("Queued {}", level->m_levelName);         

                    GJSearchObject* searchObject = GJSearchObject::create(SearchType::Search, std::to_string(level->m_levelID.value()));
                    levelSearch->getGJLevels21(searchObject); // search for the level id

                    m_queuedLevelList.erase(m_queuedLevelList.begin()); // remove the first element in the vector
                    log::info("Removed {} from queue", level->m_levelName);
                }
            }
        },
        std::chrono::seconds(loopInterval) // The interval is defined by the user
    );

    m_loopTaskID = recurringTask->k_task_id;

    m_scheduler.addTask(std::move(recurringTask));
}

std::vector<LevelCell*> QueueRequests::getLockedQueuedLevelList() {
    std::vector<LevelCell*> finalVector;

    for (size_t i = 0; i < m_queuedLevelList.size(); i++)
    {
        finalVector.push_back(m_queuedLevelList.at(i).lock());
    }
    
    return finalVector;
}

std::vector<LevelCell*> QueueRequests::getLockedTempQueuedLevelList() {
    std::vector<LevelCell*> finalVector;

    for (size_t i = 0; i < m_tempQueuedLevelList.size(); i++)
    {
        finalVector.push_back(m_tempQueuedLevelList.at(i).lock());
    }
    
    return finalVector;
}


/*
@param isEqual instead of returning n > queuedLevelList->count(), it will return n >= queuedLevelList->count()
*/
bool QueueRequests::isQueuedLevelsSuperiorToN(int n, bool isEqual = false) {
    if (!isEqual) {
        return m_queuedLevelList.size() > n;
    } else {
        return isQueuedLevelsSuperiorToN(n - 1);
    }
}

void QueueRequests::clearQueue() {
    m_queuedLevelList.clear();
}

void QueueRequests::clearTempQueue() {
    m_tempQueuedLevelList.clear();
}