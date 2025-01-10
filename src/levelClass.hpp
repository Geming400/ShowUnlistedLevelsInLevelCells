// This file has the worst name ever but finding good names is hard :(
#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class LevelClass { // not using any constructors because i'm lazy :)
public:
    GJGameLevel* level;
    LevelCell* levelCell;
    CCSprite* clockSprite;
};