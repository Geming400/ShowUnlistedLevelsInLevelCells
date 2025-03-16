#pragma once

#include "../queue/queueRequests.hpp"

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

std::string const COMMON_SECRET = "Wmfd2893gb7";
//std::string const URL = "http://www.boomlings.com/database/getGJLevels21.php";
std::string const URL = "https://gdbrowser.com/api/search";

class LevelSearch {
public:
    void getGJLevels21(GJSearchObject* searchObject);
private:
    void hideClockIcon(int levelID);

    WeakRef<LevelCell> getLevelCell(int levelID);

    EventListener<web::WebTask> m_listener;

    int m_levelID = 0;
};