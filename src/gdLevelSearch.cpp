#include "gdLevelSearch.hpp"
#include "levelInfos.hpp"
#include "fadeValues.hpp"
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

LevelSearch::LevelSearch(CCSprite* clockSprite, QueueRequests* queueRequestInstance) {
    this->m_clockSprite = clockSprite;
    this->m_queueRequestInstance = queueRequestInstance;
}

std::string removeAfterString(std::string rawResponseString, std::string removeAfter = "#") {
    std::string newRawResponseString = rawResponseString;

    newRawResponseString = newRawResponseString.substr(0, newRawResponseString.find(removeAfter)); // remove everything after the removeAfter

    return newRawResponseString;
}

/*
Similar to:

```py
def sep(values: list) -> dict:
    to_return = {}
    for i in range(0, int(len(values) / 2)):
        to_return[i] = values[i + 1]
    return to_return
```
*/
matjson::Value LevelSearch::rawLevelIntoArray(std::string rawResponseString) {
    auto rawLevels = utils::string::split(rawResponseString, "|");

    matjson::Value jsonObj;

    for (size_t i = 0; i < static_cast<size_t>(rawLevels.size() / 2); i++)
    {
        std::string key = rawLevels.at(i);
        std::string value = rawLevels.at(i + 1);
        jsonObj.set(key, value);
    }
    
    return jsonObj;
}

std::vector<GJGameLevel*> LevelSearch::parseRawLevels(std::string rawLevels) {
    std::string levels = rawLevels;
    std::vector<std::string> splittedLevels;
    std::vector<matjson::Value> levelArray;
    std::vector<GJGameLevel*> gameLevelsArray;

    levels = removeAfterString(rawLevels);
    splittedLevels = utils::string::split(levels, "|");

    int numOfLevels = splittedLevels.size();

    for (size_t i = 0; i < numOfLevels; i++)
    {
        std::string level = splittedLevels.at(i);
        log::info("level = {}", level);
        levelArray.push_back(rawLevelIntoArray(level));
    }
    
    return gameLevelsArray;
}

void LevelSearch::hideClockIcon(LevelClass levelClass) {
    LevelInfos::addQueuedLevel(levelClass.level);

    m_queueRequestInstance->removeLevelFromTempQueue(levelClass);
    auto clockSprite = m_clockSprite;
    auto fade = CCFadeOut::create(Fades::clockFadeOutTime);
    if (clockSprite) {
        clockSprite->runAction(fade);
    } else {
        log::error("Didn't found 'clockSprite'");
    }
    //clockSprite->setVisible(false);
}

void LevelSearch::getGJLevels21(GJSearchObject* searchObject, LevelClass levelClass) {
    log::debug("LevelSearch::getGJLevels21()");
    web::WebRequest req = web::WebRequest();
    std::vector<GJGameLevel*> levels;
    GJGameLevel* level = GJGameLevel::create();

    /*matjson::Value json = matjson::Value();
    json.set("secret", LevelSearch::COMMON_SECRET); // add the common secret
    json.set("type", static_cast<int>(searchObject->m_searchType));
    json.set("str", searchObject->m_searchQuery);
    req.bodyJSON(json);*/
    // not including the GJP2 to be sure to not see unlisted + friends only levels

    std::string body = std::format("secret={}&type={}&str={}",
        LevelSearch::COMMON_SECRET,
        static_cast<int>(searchObject->m_searchType),
        searchObject->m_searchQuery);

    req.bodyString(body);
    req.userAgent("");
    
    // log::warn("{}", json.dump());
    log::warn("{}", body);

    req.timeout(std::chrono::seconds(5)); // set the timeout to 5 seconds

    m_listener.bind([this, levelClass] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                if (res->ok()) { // original: res->ok()   |   for debug reasons
                    // do stuff
                    log::info("{}", res->string().unwrapOr("Uh oh!"));
                } else {
                    log::error("Request code is not 2xx"); // = no internet (probably) (robtop's servers dosen't return an actual status code by themselves) or the request failed (somehow :broken_hearth:)
                }
                hideClockIcon(levelClass);
            } else if (e->isCancelled()) {
                log::error("The request was cancelled... So sad :(");
                hideClockIcon(levelClass);
            }
        });

    m_listener.setFilter(req.post(LevelSearch::URL));
}