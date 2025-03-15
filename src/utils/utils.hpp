#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace fades {
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

namespace ids {
    std::string const CLOCK_SPRITE_ID = "clock-sprite"_spr;
    std::string const UNLISTED_SPRITE_ID = "unlisted-icon.png"_spr;
    std::string const FRIEND_ONLY_SPRITE_ID = "friend-only-icon.png"_spr;
}

namespace levelCells {
    //void updateLevelCell(LevelCell* levelCell);

    bool isMaybeFriendsOnly(std::string levelResponse);

    // For main.cpp :

    CCPoint const firstPos = {340, 10}; // the first pos, aka it's on the left side
    CCPoint const secondPos = {325, 10}; // the second pos, aka it's on the left side but a little bit less

    std::unordered_set<SearchType> const allowedTypes = { // TODO: Try implemeting that in the future
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