#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace fades {
    struct Fades {
        static inline float clockFadeOutTime = 0.5f;
        static inline float iconsFadeInTime = 0.8f;
    };

    struct FadeTo {
        static inline unsigned int iconsFadeTo = 128;
    };
}

namespace ids {
    const std::string CLOCK_SPRITE_ID = "clock-sprite"_spr;
    const std::string UNLISTED_SPRITE_ID = "unlisted-icon.png"_spr;
    const std::string FRIEND_ONLY_SPRITE_ID = "friend-only-icon.png"_spr;
}

namespace levelCells {
    //void updateLevelCell(LevelCell* levelCell);

    //bool isMaybeFriendsOnly(std::string levelResponse);

    // For main.cpp :

    const CCPoint firstPos = {340, 10}; // the first pos, aka it's on the left side
    const CCPoint secondPos = {325, 10}; // the second pos, aka it's on the left side but a little bit less

    const std::unordered_set<SearchType> allowedTypes = { // TODO: Try implemeting that in the future
        SearchType::Downloaded,
        SearchType::FavouriteLevels,
        SearchType::MyLevels,
        // SearchType::Reported, // wth is this
        SearchType::SavedLevels,
        SearchType::Search
    };
}

namespace misc {
    template<class Key, class Value>
    std::vector<Value> getValuesFromMap(std::map<Key, Value> map) {
        std::vector<Value> values;
    
        for (auto [_, v] : map) { values.push_back(v); }
    
        return values;
    }

    std::map<std::string, std::string> gdStringResponseToMap(std::string res, std::string sep);

    void log_debug(std::string s);
}