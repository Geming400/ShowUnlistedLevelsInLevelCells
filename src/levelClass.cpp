#include "levelClass.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

bool LevelClass::operator==(const LevelClass& givenClass) const {
    return level == givenClass.level;
}