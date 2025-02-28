#include "gdLevelSearch.hpp"
#include "../level/levelInfos.hpp"
#include "../utils/utils.hpp"

/*
@param p0 this CCArray contains a list of GJGameLevel*
@param p1 this is the search query, equal to GJSearchObject->getkey()
void LMD::loadLevelsFinished(CCArray* p0, char const* p1) {
    log::info("LMD::loadLevelsFinished()");

    CCArray* levels = p0;

    if (levels) {
        if (levels->count() > 0) {
            GJGameLevel* firstLevel = typeinfo_cast<GJGameLevel*>(levels->objectAtIndex(0)); // we are sure there will be only 1 level because when searching for an id
            if (!firstLevel) { // if it dosen't find the level (firstLevel = nullptr) then it's unlisted + friend only
                LevelInfos::saveCustomLevelInfos(firstLevel, true, true, levelCell); // unlisted = true, friendOnly = true
            }
        } else {
            log::info("found 0 levels");
        }
    } else {
        log::info("No level found in research");
    }

    GameLevelManager::sharedState()->m_levelManagerDelegate = originalDelegate;
    delete this;
}
*/

void LevelSearch::hideClockIcon(int levelID) {
    WeakRef<LevelCell> _levelCell = getLevelCell(levelID);
    if (QueueRequests::get()->getTempQueue().size() == 0) {
        log::debug("empty");
    }
    if (!_levelCell) {
        log::error("Level cell = nullptr");
    }
    Ref<LevelCell> levelCell = _levelCell.lock();
    if (!levelCell) { return; }
    LevelInfos::addQueuedLevel(levelCell->m_level);

    QueueRequests::get()->removeLevelFromTempQueue(levelCell);
    log::debug("REMOVE FROM TEMP QUEUE");
    CCFadeTo* fade = CCFadeTo::create(Fades::Fades::clockFadeOutTime, 0); // to 0 opacity

    

    CCNode* sprite = levelCell->getChildByID(Ids::CLOCK_SPRITE_ID);
    if (sprite) { sprite->runAction(fade); }
    //clockSprite->setVisible(false);
}

WeakRef<LevelCell> LevelSearch::getLevelCell(int levelID) {
    return QueueRequests::get()->getLevelCellFromLevelID(levelID, true);
}

void LevelSearch::getGJLevels21(GJSearchObject* searchObject) {
    web::WebRequest req = web::WebRequest();
    std::vector<GJGameLevel*> levels;
    std::string _levelID = searchObject->m_searchQuery;
    int levelID = std::stoi(_levelID); // = _levelID to int

    /*matjson::Value json = matjson::Value();
    json.set("secret", LevelSearch::COMMON_SECRET); // add the common secret
    json.set("type", static_cast<int>(searchObject->m_searchType));
    json.set("str", searchObject->m_searchQuery);
    req.bodyJSON(json);*/
    // not including the GJP2 to be sure to not see unlisted + friends only levels

    std::string body = fmt::format("secret={}&type={}&str={}",
        COMMON_SECRET,
        static_cast<int>(searchObject->m_searchType),
        searchObject->m_searchQuery);

    log::warn("{}", body);

    req.bodyString(body);
    req.header("Content-Type", "application/x-www-form-urlencoded");
    req.userAgent("");

    req.timeout(std::chrono::seconds(5)); // set the timeout to 5 seconds
    
    LevelCell* levelCell = getLevelCell(levelID).lock();

    m_listener.bind([this, levelCell, levelID] (web::WebTask::Event* e) {
        if (!m_alreadyRan) {
            if (web::WebResponse* res = e->getValue()) {
                if (res->ok()) { // original: res->ok()   |   for debug reasons
                    std::string resString = res->string().unwrapOr("-1");
                    
                    log::debug("resString = {}", resString);

                    if (resString == "-1") { // there's no level being found (aka the level is friends only)
                        log::info("if (resString == -1)");
                        if (levelCell) {
                            LevelInfos::saveCustomLevelInfos(levelCell, true, true);
                        }
                    }
                } else {
                    log::error("Request code is not 2xx"); // = no internet (probably) (robtop's servers dosen't return an actual status code by themselves) or the request failed (somehow :broken_hearth:)
                }
            } else if (e->isCancelled()) {
                log::error("The request was cancelled... So sad :(");
            }
            
            log::info("m_alreadyRan = false");
            m_alreadyRan = true;
            hideClockIcon(levelID);
        } else {
            log::info("m_alreadyRan = true");
        }
    });

    m_listener.setFilter(req.post(URL));
}