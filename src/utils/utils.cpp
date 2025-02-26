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

CCArray* CocosUtils::getValuesFromDict(CCDictionaryExt<int, LevelCell> dict) {
    CCArray* values = CCArray::create();

    for (auto [_, v] : dict) { values->addObject(v); }
    //for (auto& [_, v] : CCDictionaryExt<LevelCell>(dict)) { values->addObject(v); }

    return values;
}