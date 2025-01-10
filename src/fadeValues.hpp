#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class Fades {
public:
    static inline auto clockFadeOut = CCFadeOut::create(1.5);
};