#pragma once

#include "../level/levelClass.hpp"
#include "../queue/queueRequests.hpp"

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
    LevelSearch(LevelCell* levelCell, QueueRequests* queueRequests);
    LevelSearch(WeakRef<LevelCell> levelCell, QueueRequests* queueRequest);

    void getGJLevels21(GJSearchObject* searchObject);
private:
    void hideClockIcon(LevelCell* levelCell);

    const std::string COMMON_SECRET = "Wmfd2893gb7";
    const std::string URL = "http://www.boomlings.com/database/getGJLevels21.php";

    EventListener<web::WebTask> m_listener;

    WeakRef<LevelCell> m_levelCell = nullptr;
    QueueRequests* m_queueRequestInstance;
};