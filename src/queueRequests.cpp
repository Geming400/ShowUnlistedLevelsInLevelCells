#include "queueRequests.hpp"
#include "levelInfos.hpp"
#include "levelClass.hpp"
#include "gdLevelSearch.hpp"

#include "../include/TaskScheduler.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

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

void QueueRequests::addLevelToQueue(LevelClass levelClass) {
    // log::debug("LevelInfos::wasAlreadyQueued(levelClass.level) = {}", std::to_string(LevelInfos::wasAlreadyQueued(levelClass.level)));
    // log::debug("isQueued(levelClass.level) = {}", std::to_string(isQueued(levelClass.level)));

    if (levelClass.level && !(LevelInfos::wasAlreadyQueued(levelClass.level) || isQueued(levelClass.level))) {
        m_queuedLevelList.push_back(levelClass);
    } else {
        log::info("Level {} won't be added to queue, as it's already in queue or was already in queue", levelClass.level->m_levelName);
    }
}

void QueueRequests::addLevelToTempQueue(LevelClass levelClass) {
    log::debug("QueueRequests::addLevelToTempQueue()");
    // log::debug("LevelInfos::wasAlreadyQueued(levelClass.level) = {}", std::to_string(LevelInfos::wasAlreadyQueued(levelClass.level)));
    // log::debug("isQueued(levelClass.level) = {}", std::to_string(isQueued(levelClass.level)));

    if (levelClass.level && !isQueuedInTempQueue(levelClass.level)) {
        m_tempQueuedLevelList.push_back(levelClass);
    } else {
        log::info("Level {} won't be added to the temp queue because the level is already in the temp queue", levelClass.level->m_levelName);
    }
}

void QueueRequests::removeLevelFromQueue(LevelClass levelClass) {
    log::debug("QueueRequests::removeLevelFromQueue()");
    if (levelClass.level && (LevelInfos::wasAlreadyQueued(levelClass.level) || isQueued(levelClass))) {
        auto pos = std::find(
                m_queuedLevelList.begin(), m_queuedLevelList.end(),
                levelClass
            );
        
        if (pos != m_queuedLevelList.end()) {
            m_queuedLevelList.erase(pos);
        }
    } else {
        log::info("Level {} won't be removed in queue, as it wasn't already in queue or wasn't already in queue", levelClass.level->m_levelName);
    }
}

void QueueRequests::removeLevelFromTempQueue(LevelClass levelClass) {
    log::debug("QueueRequests::removeLevelFromTempQueue()");
    if (levelClass.level && isQueuedInTempQueue(levelClass.level)) {
        log::debug("Before std::find -> m_tempQueuedLevelList size: {}", m_tempQueuedLevelList.size());
        //log::debug("removeLevelFromTempQueue -> Starting std::find with level: {}",
        //   levelClass.level ? levelClass.level->m_levelName : "nullptr");
        auto pos = std::find(
                m_tempQueuedLevelList.begin(), m_tempQueuedLevelList.end(),
                levelClass
            );
        
        if (pos != m_tempQueuedLevelList.end()) {
            m_tempQueuedLevelList.erase(pos);
        } else {
            for (size_t i = 0; i < m_tempQueuedLevelList.size(); i++)
            {
                auto level = m_tempQueuedLevelList.at(i).level;
                auto name = level->m_levelName;
            }
            
            log::debug("if (pos == m_tempQueuedLevelList.end())");
        }
    } else {
        log::info("Level {} won't be removed in queue, as it wasn't already in the temp queue", levelClass.level->m_levelName);
    }
}

bool QueueRequests::isQueuedInTempQueue(GJGameLevel* level) {
    int levelID = level->m_levelID.value();

    for (size_t i = 0; i < m_tempQueuedLevelList.size(); i++)
	{   
        GJGameLevel* queuedLevel = m_tempQueuedLevelList.at(i).level;
        int queuedLevelID = queuedLevel->m_levelID.value();

		if (levelID == queuedLevelID) {
			return true;
		}
	}
    
    return false;
}

bool QueueRequests::isQueuedInTempQueue(LevelClass levelClass) {
    int levelID = levelClass.level->m_levelID.value();

    for (size_t i = 0; i < m_tempQueuedLevelList.size(); i++)
	{   
        GJGameLevel* queuedLevel = m_tempQueuedLevelList.at(i).level;
        int queuedLevelID = queuedLevel->m_levelID.value();

		if (levelID == queuedLevelID) {
			return true;
		}
	}
    
    return false;
}

bool QueueRequests::isQueued(GJGameLevel* level) {
    int levelID = level->m_levelID.value();

    for (size_t i = 0; i < m_queuedLevelList.size(); i++)
	{
        GJGameLevel* queuedLevel = m_queuedLevelList.at(i).level;
        int queuedLevelID = queuedLevel->m_levelID.value();

		if (levelID == queuedLevelID) {
			return true;
		}
	}
    
    return false;
}

bool QueueRequests::isQueued(LevelClass levelClass) {
    int levelID = levelClass.level->m_levelID.value();

    for (size_t i = 0; i < m_queuedLevelList.size(); i++)
	{
        GJGameLevel* queuedLevel = m_queuedLevelList.at(i).level;
        int queuedLevelID = queuedLevel->m_levelID.value();

		if (levelID == queuedLevelID) {
			return true;
		}
	}
    
    return false;
}

void QueueRequests::startLoop() {
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

                    LevelClass levelClass = m_queuedLevelList.at(0); // always get the first element
                    addLevelToTempQueue(levelClass);

                    auto levelSearch = new LevelSearch(levelClass.clockSprite, this); // ik it's very bad to pass the entire instance as an argument, but I don't really know how to do lamba function :/

                    GJGameLevel* level = levelClass.level;
                    log::info("Queued {}", level->m_levelName);         

                    GJSearchObject* searchObject = GJSearchObject::create(SearchType::Search, std::to_string(level->m_levelID.value()));
                    levelSearch->getGJLevels21(searchObject, levelClass); // search for the level id

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