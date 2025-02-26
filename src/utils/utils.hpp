#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace Fades {
    class Fades {
    public:
        static inline float clockFadeOutTime = 0.5f;
        static inline float iconsFadeInTime = 0.8f;
    };

    class FadeTo {
    public:
        static inline unsigned int iconsFadeTo = 128;
    };
}

namespace Ids {
    const std::string CLOCK_SPRITE_ID = "clock-sprite"_spr;
    const std::string UNLISTED_SPRITE_ID = "unlisted-icon.png"_spr;
    const std::string FRIEND_ONLY_SPRITE_ID = "friend-only-icon.png"_spr;
}

namespace LevelCells {
    void updateLevelCell(LevelCell* levelCell);
}

namespace Cocos {
    CCArray* getObjectsFromDict(CCDictionary* dict);
}