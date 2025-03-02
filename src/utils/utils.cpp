#include "utils.hpp"
#include "../level/levelInfos.hpp"
#include "../queue/queueRequests.hpp"

void LevelCells::updateLevelCell(LevelCell* levelCell) {
    CCSprite* clockSprite = typeinfo_cast<CCSprite*>(levelCell->getChildByID(Ids::CLOCK_SPRITE_ID));
    CCSprite* unlistedSprite = typeinfo_cast<CCSprite*>(levelCell->getChildByID(Ids::UNLISTED_SPRITE_ID));
    CCSprite* friendOnlySprite = typeinfo_cast<CCSprite*>(levelCell->getChildByID(Ids::FRIEND_ONLY_SPRITE_ID));
    
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
}

bool LevelCells::isUnlistedOrFriendsOnly(std::string levelResponse) {
    std::map<std::string, std::string> mappedResponse = Misc::gdStringResponseToMap(levelResponse, ":");

    std::vector<std::string> key35 = geode::utils::string::split(mappedResponse["35"], "#"); // see https://wyliemaster.github.io/gddocs/#/resources/client/level (k35)
    return key35.at(0) == "0";
}

bool LevelCells::isFriendsOnly(std::string levelResponse) {
    if (!isUnlistedOrFriendsOnly(levelResponse)) {
        return Misc::gdStringResponseToMap(levelResponse, ":").contains("4"); // level string key is 4
    } else {
        return false;
    }
}

void Misc::log_debug(std::string s) {
    if (Mod::get()->getSettingValue<bool>("show-debug-logs")) {
        log::debug("{}", s);
    }
}

bool Misc::isEven(int n) {
    return n % 2 == 0;
}

std::map<std::string, std::string> Misc::gdStringResponseToMap(std::string res, std::string sep){
    std::vector<std::string> splittedRes = geode::utils::string::split(res, sep);

    std::map<std::string, std::string> finalMap;

    for (size_t i = 0; i < splittedRes.size(); i = i+2)
    {
        std::string key = splittedRes.at(i);
        std::string value = splittedRes.at(i + 1);

        finalMap[key] = value;
    }
    
    return finalMap;
}