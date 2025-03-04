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

    bool isFriendsOnly(std::string levelResponse);
}

namespace Misc {
    template<class Key, class Value>
    std::vector<Value> getValuesFromMap(std::map<Key, Value> map) {
        std::vector<Value> values;
    
        for (auto [_, v] : map) { values.push_back(v); }
    
        return values;
    }

    bool isEven(int n);

    std::map<std::string, std::string> gdStringResponseToMap(std::string res, std::string sep);

    void log_debug(std::string s);
}