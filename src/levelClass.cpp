#include "levelClass.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

GJGameLevel* LevelClass::getLevel() const {
    if (auto obj = levelCell.lock()) {
        return obj->m_level;
    }

    return nullptr;
}

CCSprite* LevelClass::getUnlistedicon() const {
    if (auto obj = levelCell.lock()) {
        return typeinfo_cast<CCSprite*>(obj->getChildByID(Ids::UNLISTED_SPRITE_ID));
    }

    return nullptr;
}

CCSprite* LevelClass::getFriendOnlyIcon() const {
    if (auto obj = levelCell.lock()) {
        return typeinfo_cast<CCSprite*>(obj->getChildByID(Ids::FRIEND_ONLY_SPRITE_ID));
    }

    return nullptr;
}

CCSprite* LevelClass::getClockIcon() const {
    if (auto obj = levelCell.lock()) {
        return typeinfo_cast<CCSprite*>(obj->getChildByID(Ids::CLOCK_SPRITE_ID));
    }

    return nullptr;
}

bool LevelClass::operator==(const LevelClass& other) const {
    return getLevel() == other.getLevel();
}