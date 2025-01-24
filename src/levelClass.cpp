#include "levelClass.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

bool LevelClass::operator==(const LevelClass& givenClass) const {
    log::debug("LevelClass::operator==() -> this.level: {}", level->m_levelName);
    log::debug("LevelClass::operator==() -> givenClass.level: {}", givenClass.level->m_levelName);
    return level == givenClass.level;
}