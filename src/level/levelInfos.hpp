#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class LevelInfos {
public:
	// unlisted levels
	static auto getUnlistedLevels();

	static bool isUnlisted(GJGameLevel* level);
	static bool isUnlisted(int levelID);

	static void addUnlistedLevel(GJGameLevel* level);
	static void addUnlistedLevel(int levelID);

	static void clearUnlistedLevels();

	// friend only levels
	static auto getFriendOnlyLevels();

	static bool isFriendOnly(GJGameLevel* level);
	static bool isFriendOnly(int levelID);

	static void addFriendOnlyLevel(GJGameLevel* level);
	static void addFriendOnlyLevel(int levelID);

	static void clearFriendOnlyLevels();

	// queued levels

	static auto getAlreadyQueuedLevels();

	static bool wasAlreadyQueued(GJGameLevel* level);
	static bool wasAlreadyQueued(int levelID);

	static void addQueuedLevel(GJGameLevel* level);
	static void addQueuedLevel(int levelID);

	static void clearAlreadyQueuedLevels();

	// add the  unlisted / friend only  levels
	static void saveLevelInfos(GJGameLevel* level);
	static void saveLevelInfos(int levelID);

	static void saveCustomLevelInfos(GJGameLevel* level, bool isUnlisted, bool isFriendOnly);
	static void saveCustomLevelInfos(int levelID, bool isUnlisted, bool isFriendOnly);
	static void saveCustomLevelInfos(LevelCell* levelCell, bool isUnlisted, bool isFriendOnly);

	static void saveInfosOfLevelsInArray(CCArray* levels);

	// other stuff

	static bool isLevelDaily(GJGameLevel* level);
	static bool isLevelRated(GJGameLevel* level);
};