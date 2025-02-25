// This file has the worst name ever but finding good names is hard :(
#pragma once

#include "../utils/utils.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class LevelClass { // not using any constructors because i'm lazy :)
public:
    WeakRef<LevelCell> levelCell;

    GJGameLevel* getLevel() const;

    CCSprite* getUnlistedicon() const;

    CCSprite* getFriendOnlyIcon() const;

    CCSprite* getClockIcon() const;

    bool operator==(const LevelClass& other) const;
};