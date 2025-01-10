#include "queueRequests.hpp"
#include "levelInfos.hpp"
#include "levelClass.hpp"
#include "gdLevelSearch.hpp"

#include "../include/TaskScheduler.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

bool hasUserToggledOnQueuedRequests() {
    return Mod::get()->getSettingValue<bool>("queue-requests");
}

void QueueRequests::addLevelToQueue(LevelClass levelClass) {
    // log::debug("LevelInfos::wasAlreadyQueued(levelClass.level) = {}", std::to_string(LevelInfos::wasAlreadyQueued(levelClass.level)));
    // log::debug("isQueued(levelClass.level) = {}", std::to_string(isQueued(levelClass.level)));
    if (!(LevelInfos::wasAlreadyQueued(levelClass.level) || isQueued(levelClass.level))) {
        m_queuedLevelList.push_back(levelClass);
    } else {
        log::info("Level {} won't be added to queue, as it's already in queue or was already in queue", levelClass.level->m_levelName);
    }
}

void QueueRequests::addLevelToTempQueue(LevelClass levelClass) {
    // log::debug("LevelInfos::wasAlreadyQueued(levelClass.level) = {}", std::to_string(LevelInfos::wasAlreadyQueued(levelClass.level)));
    // log::debug("isQueued(levelClass.level) = {}", std::to_string(isQueued(levelClass.level)));
    if (!(LevelInfos::wasAlreadyQueued(levelClass.level) || isQueued(levelClass.level))) {
        m_tempQueuedLevelList.push_back(levelClass);
    } else {
        log::info("Level {} won't be added to queue, as it's already in queue or was already in queue", levelClass.level->m_levelName);
    }
}

bool QueueRequests::isQueued(LevelClass levelClass, bool isFromTempQueue = false) {
    int levelID = levelClass.level->m_levelID.value();

    for (size_t i = 0; i < m_queuedLevelList.size(); i++)
	{   
        std::vector<LevelClass> level_list;
        if (isFromTempQueue) {
            auto level_list = m_tempQueuedLevelList;
        } else {
            auto level_list = m_queuedLevelList;
        }

        GJGameLevel* queuedLevel = level_list.at(i).level;
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

                    LevelClass lvlClass = m_queuedLevelList.at(0); // always get the first element
                    
                    auto levelSearch = new LevelSearch(lvlClass.levelCell, lvlClass.clockSprite);

                    GJGameLevel* level = lvlClass.level;
                    log::info("Queued {}", level->m_levelName);         

                    GJSearchObject* searchObject = GJSearchObject::create(SearchType::Search, std::to_string(level->m_levelID.value()));
                    levelSearch->getGJLevels21(searchObject);
                    // gameLevelManager->getOnlineLevels(searchObject); // search for the level ID

                    m_queuedLevelList.erase(m_queuedLevelList.begin()); // remove the first element in the vector
                    log::info("Removed {} from queue", level->m_levelName);

                    LevelInfos::addQueuedLevel(level);
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