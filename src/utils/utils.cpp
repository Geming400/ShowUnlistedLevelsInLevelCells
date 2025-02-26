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

/*
@note Only works for CCDictionaries that use int keys. If it uses gd::string keys instead, it will return `nullptr`
*/
CCArray* Cocos::getObjectsFromDict(CCDictionary* dict) {
    try {
        dict->objectForKey(0);
    } catch (...) {
        return nullptr; // it returns nullptr if objectForKey() did an assert
    }

    CCArray* finalArray = CCArray::create();

    for (size_t i = 0; i < dict->count(); i++)
    {
        auto obj = dict->objectForKey(i);

        finalArray->addObject(obj);
    }

    return finalArray;
}