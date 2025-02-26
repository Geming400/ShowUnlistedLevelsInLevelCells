#pragma once

#include "../level/levelClass.hpp"
#include "../queue/queueRequests.hpp"

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class LevelSearch {
public:
    LevelSearch(LevelCell* levelCell, QueueRequests* queueRequests);
    LevelSearch(WeakRef<LevelCell> levelCell, QueueRequests* queueRequest);

    void getGJLevels21(GJSearchObject* searchObject);
private:
    void hideClockIcon();

    const std::string COMMON_SECRET = "Wmfd2893gb7";
    const std::string URL = "http://www.boomlings.com/database/getGJLevels21.php";

    EventListener<web::WebTask> m_listener;

    WeakRef<LevelCell> m_levelCell = nullptr;
    QueueRequests* m_queueRequestInstance;
};