#include "utils.hpp"
#include "../level/levelInfos.hpp"
#include "../queue/queueRequests.hpp"

/*void levelCells::updateLevelCell(LevelCell* levelCell) {
    CCSprite* clockSprite = typeinfo_cast<CCSprite*>(levelCell->getChildByID(ids::CLOCK_SPRITE_ID));
    CCSprite* unlistedSprite = typeinfo_cast<CCSprite*>(levelCell->getChildByID(ids::UNLISTED_SPRITE_ID));
    CCSprite* friendOnlySprite = typeinfo_cast<CCSprite*>(levelCell->getChildByID(ids::FRIEND_ONLY_SPRITE_ID));
    
    if (!(clockSprite && unlistedSprite && friendOnlySprite)) {
        return;
    }

    if (LevelInfos::wasAlreadyQueued(levelCell->m_level) || QueueRequests::get()->isQueued(levelCell) || QueueRequests::get()->isQueuedInTempQueue(levelCell)) {
        clockSprite->setOpacity(0);
        clockSprite->setVisible(false);
    }
    if (LevelInfos::isUnlisted(levelCell->m_level)) {
        unlistedSprite->setOpacity(0);
        unlistedSprite->setVisible(false);
    }
    if (LevelInfos::isFriendOnly(levelCell->m_level)) {
        friendOnlySprite->setOpacity(0);
        friendOnlySprite->setVisible(false);
    }
}*/

bool levelCells::isMaybeFriendsOnly(std::string levelResponse) {
    if (levelResponse == "-1") {
        return true;
    } else { // the code below is because of ip stuff (read https://discord.com/channels/911701438269386882/979402752121765898/1346204609730842665)
             // The big issue with this, is that we can't know if the level is either unlisted or friends only because of robtop's stupid ip stuff. So we need to assume
        std::map<std::string, std::string> mappedResponse = Misc::gdStringResponseToMap(levelResponse, ":");

        std::vector<std::string> key35 = geode::utils::string::split(mappedResponse["35"], "#"); // see https://wyliemaster.github.io/gddocs/#/resources/client/level (k35)
        return key35.at(0) == "0";
    }
}

void Misc::log_debug(std::string s) {
    if (Mod::get()->getSettingValue<bool>("show-debug-logs") && Mod::get()->getSettingValue<bool>("enable-debug-utilities")) {
        log::debug("{}", s);
    }
}

std::map<std::string, std::string> Misc::gdStringResponseToMap(std::string res, std::string sep){
    std::vector<std::string> splittedRes = geode::utils::string::split(res, sep);

    std::map<std::string, std::string> finalMap;

    for (size_t i = 0; i < splittedRes.size(); i = i + 2)
    {
        std::string key = splittedRes.at(i);
        std::string value = splittedRes.at(i + 1);

        finalMap[key] = value;
    }
    
    return finalMap;
}