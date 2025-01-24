#pragma once

#include "levelClass.hpp"
#include "queueRequests.hpp"

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

/*
// thank CVolton for this: https://discord.com/channels/911701438269386882/979402752121765898/1323744042269278310
class LMD : public LevelManagerDelegate { // I don't even know how to name this
public:
    void loadLevelsFinished(CCArray* p0, char const* p1) override;
    LevelManagerDelegate* originalDelegate;
    LevelCell* levelCell;
};
*/

class LevelSearch {
public:
    LevelSearch(CCSprite* clockSprite, QueueRequests* queueRequestInstance);
    void getGJLevels21(GJSearchObject* searchObject, LevelClass levelClass);
private:
    std::vector<GJGameLevel*> parseRawLevels(std::string rawLevels);
    matjson::Value rawLevelIntoArray(std::string rawResponseString);
    void hideClockIcon(LevelClass levelClass);

    const std::string COMMON_SECRET = "Wmfd2893gb7";
    const std::string URL = "http://www.boomlings.com/database/getGJLevels21.php";

    EventListener<web::WebTask> m_listener;

    LevelCell* m_levelCell;
    CCSprite* m_clockSprite;
    QueueRequests* m_queueRequestInstance;
};