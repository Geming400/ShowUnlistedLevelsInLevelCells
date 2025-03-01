#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class LevelInfos {
public:
	// unlisted levels
	static auto getUnlistedLevels();

	static bool isUnlisted(GJGameLevel* level);

	static void addUnlistedLevel(GJGameLevel* level);

	static void clearUnlistedLevels();

	// friend only levels
	static auto getFriendOnlyLevels();

	static bool isFriendOnly(GJGameLevel* level);

	static void addFriendOnlyLevel(GJGameLevel* level);

	static void clearFriendOnlyLevels();

	// queued levels

	static auto getAlreadyQueuedLevels();

	static bool wasAlreadyQueued(GJGameLevel* level);

	static void addQueuedLevel(GJGameLevel* level);

	static void clearAlreadyQueuedLevels();

	// add the  unlisted / friend only  levels
	static void saveLevelInfos(GJGameLevel* level);

	static void saveCustomLevelInfos(GJGameLevel* level, bool isUnlisted, bool isFriendOnly);
	static void saveCustomLevelInfos(LevelCell* levelCell, bool isUnlisted, bool isFriendOnly);

	static void saveInfosOfLevelsInArray(CCArray* levels);

	// other stuff

	static bool isLevelDaily(GJGameLevel* level);
};