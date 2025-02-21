#pragma once

#include "levelClass.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class LevelInfos {
public:
	// unlisted levels
	static auto getUnlistedLevels();

	static bool isUnlisted(GJGameLevel* level);

	static void addUnlistedLevel(GJGameLevel* level);

	// friend only levels
	static auto getFriendOnlyLevels();

	static bool isFriendOnly(GJGameLevel* level);

	static void addFriendOnlyLevel(GJGameLevel* level);

	// queued levels

	static auto getAlreadyQueuedLevels();

	static bool wasAlreadyQueued(GJGameLevel* level);

	static void addQueuedLevel(GJGameLevel* level);

	// add the  unlisted / friend only  levels
	static void saveLevelInfos(GJGameLevel* level);

	static void saveCustomLevelInfos(GJGameLevel* level, bool isUnlisted, bool isFriendOnly);
	static void saveCustomLevelInfos(LevelClass levelClass, bool isUnlisted, bool isFriendOnly);

	static void saveInfosOfLevelsInArray(CCArray* levels);

	// other stuff

	static bool isLevelDaily(GJGameLevel* level);
};