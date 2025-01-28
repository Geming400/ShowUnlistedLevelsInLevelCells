#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class Fades {
public:
    static inline float clockFadeOutTime = 0.5f;
    static inline float iconsFadeInTime = 0.8f;
};

class FadeTo {
public:
    static inline unsigned int iconsFadeTo = 128;
};