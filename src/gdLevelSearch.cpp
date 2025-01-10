#include "gdLevelSearch.hpp"
#include "levelInfos.hpp"
#include "fadeValues.hpp"

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

LevelSearch::LevelSearch(LevelCell* levelCell, CCSprite* clockIcon) {
    this->m_levelCell = levelCell;
    this->m_clockIcon = clockIcon;
}

std::vector<std::string> split(std::string str, std::string delimiter) {
    std::vector<std::string> tokens;
    size_t pos {};

    for (size_t fd = 0; (fd = str.find(delimiter, pos)) != std::string::npos; pos = fd + delimiter.size()) {
        tokens.emplace_back(str.data() + pos, str.data() + fd);
    }

    tokens.emplace_back(str.data() + pos, str.data() + str.size());
    return tokens;
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
    auto rawLevels = split(rawResponseString, "|");

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
    splittedLevels = split(levels, "|");

    int numOfLevels = splittedLevels.size();

    for (size_t i = 0; i < numOfLevels; i++)
    {
        std::string level = splittedLevels.at(i);
        log::info("level = {}", level);
        levelArray.push_back(rawLevelIntoArray(level));
    }
    
    return gameLevelsArray;
}

void LevelSearch::hideClockIcon() {
    auto clockSprite = m_levelCell->getChildByID("clock-sprite"_spr);
    auto fade = Fades::clockFadeOut;
    if (clockSprite) {
        clockSprite->runAction(fade);
    } else {
        log::warn("Didn't found 'clockSprite'");
    }
    //clockSprite->setVisible(false);
}

void LevelSearch::getGJLevels21(GJSearchObject* searchObject) {
    log::debug("LevelSearch::getGJLevels21()");
    web::WebRequest req = web::WebRequest();
    std::vector<GJGameLevel*> levels;
    GJGameLevel* level = GJGameLevel::create();

    matjson::Value json = matjson::Value();
    json.set("secret", LevelSearch::COMMON_SECRET); // add the common secret
    json.set("type", static_cast<int>(searchObject->m_searchType));
    json.set("str", searchObject->m_searchQuery);
    // not including the GJP2 to be sure to not see unlisted + friends only levels

    req.userAgent("");
    req.bodyJSON(json);

    req.timeout(std::chrono::seconds(5)); // set the timeout to 5 seconds

    m_listener.bind([this] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                if (!res->ok()) { // original: res->ok()   |   for debug reasons
                    // do stuff
                    log::info("{}", res->string().unwrapOr("Uh oh!"));
                } else {
                    log::warn("Request code is not 2xx"); // = no internet (robtop's servers dosen't return an actual status code by themselves)
                }
                hideClockIcon();
            } else if (e->isCancelled()) {
                log::error("The request was cancelled... So sad :(");
                hideClockIcon();
            }
        });

    m_listener.setFilter(req.post(LevelSearch::URL));
}