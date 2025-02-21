#include "gdLevelSearch.hpp"
#include "levelInfos.hpp"
#include "utils.hpp"
#include "levelClass.hpp"
#include "queueRequests.hpp"

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

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

LevelSearch::LevelSearch(LevelClass levelClass, QueueRequests* queueRequest) {
    m_levelClass = levelClass;
    m_queueRequestInstance = queueRequest;
}

void LevelSearch::hideClockIcon(LevelClass levelClass) {
    LevelInfos::addQueuedLevel(levelClass.levelCell.lock()->m_level);

    m_queueRequestInstance->removeLevelFromTempQueue(levelClass);
    CCFadeTo* fade = CCFadeTo::create(Fades::Fades::clockFadeOutTime, 0); // to 0 opacity

    

    if (auto obj = m_levelClass.getClockIcon()) {
        obj->runAction(fade);
    } else {
        log::error("Didn't found 'clockSprite'");
    }
    //clockSprite->setVisible(false);
}

void LevelSearch::getGJLevels21(GJSearchObject* searchObject) {
    web::WebRequest req = web::WebRequest();
    std::vector<GJGameLevel*> levels;
    GJGameLevel* level = GJGameLevel::create();

    /*matjson::Value json = matjson::Value();
    json.set("secret", LevelSearch::COMMON_SECRET); // add the common secret
    json.set("type", static_cast<int>(searchObject->m_searchType));
    json.set("str", searchObject->m_searchQuery);
    req.bodyJSON(json);*/
    // not including the GJP2 to be sure to not see unlisted + friends only levels

    std::string body = fmt::format("secret={}&type={}&str={}",
        LevelSearch::COMMON_SECRET,
        static_cast<int>(searchObject->m_searchType),
        searchObject->m_searchQuery);

    log::warn("{}", body);

    req.bodyString(body);
    req.header("Content-Type", "application/x-www-form-urlencoded");
    req.userAgent("");

    req.timeout(std::chrono::seconds(5)); // set the timeout to 5 seconds
    
    LevelClass levelClass = m_levelClass;

    m_listener.bind([this, levelClass] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                if (!res->ok()) { // original: res->ok()   |   for debug reasons
                    // do stuff
                    std::string resString = res->string().unwrapOr("-1");
                    
                    log::debug("resString = {}", resString);

                    if (resString == "-1") { // there's no level being found (aka the level is friends only)
                        log::info("if (resString == -1)");
                        LevelInfos::saveCustomLevelInfos(levelClass, true, true);
                    }
                } else {
                    log::error("Request code is not 2xx"); // = no internet (probably) (robtop's servers dosen't return an actual status code by themselves) or the request failed (somehow :broken_hearth:)
                }
            } else if (e->isCancelled()) {
                log::error("The request was cancelled... So sad :(");
            }
            hideClockIcon(levelClass);
        });

    m_listener.setFilter(req.post(LevelSearch::URL));
}