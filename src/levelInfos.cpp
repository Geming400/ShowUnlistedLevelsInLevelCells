#include "levelInfos.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

const std::string EMPTY_ARRAY = matjson::Value::array().dump(matjson::NO_INDENTATION);
const int DEFAULT_NO_LEVEL_ID = 0;

const std::string unlistedLevelsKey = "unlisted-levels";
const std::string friendOnlyLevelsKey = "friend-only-levels";
const std::string queuedLevelsKey = "queued-levels";

// unlisted levels
auto LevelInfos::getUnlistedLevels() {
	auto parsedUnlistedLevels = matjson::parse(Mod::get()->getSavedValue<std::string>(unlistedLevelsKey, EMPTY_ARRAY)); // get the unlisted levels from the saved key and turn it into a Geode::Result instance
	matjson::Value unwrapedUnlistedLevels = parsedUnlistedLevels.unwrapOr(EMPTY_ARRAY); // get the unlisted levels but in a matjson array
	return unwrapedUnlistedLevels;
}

bool LevelInfos::isUnlisted(GJGameLevel* level) {
	int levelID = level->m_levelID.value();
	
	matjson::Value unlistedLevels = LevelInfos::getUnlistedLevels();

	for (size_t i = 0; i < unlistedLevels.size(); i++) // iterate trought each unlisted levels
	{
		int UnlistedlevelID = unlistedLevels.get<int>(i).unwrapOr(DEFAULT_NO_LEVEL_ID); // get the level id

		if (levelID == UnlistedlevelID) {
			return true;
		}
	}

	return false;
}

void LevelInfos::addUnlistedLevel(GJGameLevel* level) {
	LevelInfos::addQueuedLevel(level);

	// if the level is already unlisted, don't add it to the unlisted level array
	if (isUnlisted(level)) {
		log::info("level {} is already in the '{}' array", level->m_levelName, unlistedLevelsKey);
		return;
	}

	int levelID = level->m_levelID.value();

	matjson::Value unlistedLevels = LevelInfos::getUnlistedLevels();
	unlistedLevels.push(levelID);

	log::debug("Refreshing the '{}' level array", unlistedLevelsKey);
	Mod::get()->setSavedValue<std::string>(unlistedLevelsKey, unlistedLevels.dump());
}

// friend only levels
auto LevelInfos::getFriendOnlyLevels() {
	auto parsedFriendOnlyLevels = matjson::parse(Mod::get()->getSavedValue<std::string>(friendOnlyLevelsKey, EMPTY_ARRAY)); // get the friend only from the saved key and turn it into a Geode::Result instance
	auto unwrapedFriendOnlydLevels = parsedFriendOnlyLevels.unwrapOr(EMPTY_ARRAY); // get the friend only levels but in a matjson array
	return unwrapedFriendOnlydLevels;
}

bool LevelInfos::isFriendOnly(GJGameLevel* level) {
	int levelID = level->m_levelID.value();

	matjson::Value unlistedLevels = getFriendOnlyLevels();

	for (size_t i = 0; i < unlistedLevels.size(); i++) // iterate trought each friend only levels
	{
		int friendOnlylevelID = unlistedLevels.get<int>(i).unwrapOr(DEFAULT_NO_LEVEL_ID); // get the level id

		if (levelID == friendOnlylevelID) {
			return true;
		}
	}

	return false;
}

void LevelInfos::addFriendOnlyLevel(GJGameLevel* level) {
	LevelInfos::addQueuedLevel(level);
	
	// if the level is already unlisted, don't add it to the unlisted level array
	if (LevelInfos::isFriendOnly(level)) {
		log::info("level {} is already in the '{}' array", level->m_levelName, friendOnlyLevelsKey);
		return;
	}

	matjson::Value friendOnlyLevels = getFriendOnlyLevels();
	friendOnlyLevels.push(level->m_levelID.value());

	log::debug("Refreshing the '{}' level array", friendOnlyLevelsKey);
	Mod::get()->setSavedValue<std::string>(friendOnlyLevelsKey, friendOnlyLevels.dump());
}

// add the  unlisted / friend only  levels
void LevelInfos::saveLevelInfos(GJGameLevel* level) {
	if (level->m_unlisted) {
		log::debug("Level is unlisted");
		addUnlistedLevel(level);
	}

	if (level->m_friendsOnly) {
		log::debug("Level is friend only");
		addFriendOnlyLevel(level);
	}
}

/*
This function is like saveLevelInfos except it won't see the content of level->m_unlisted and level->m_friendsOnly
It's the user who defines if the level is unlisted or friend only
*/
void LevelInfos::saveCustomLevelInfos(GJGameLevel* level, bool isUnlisted, bool isFriendOnly) {
	if (isUnlisted) {
		addUnlistedLevel(level);
	}

	if (isFriendOnly) {
		addFriendOnlyLevel(level);
	}
}

/*
This function is like saveLevelInfos except it won't see the content of level->m_unlisted and level->m_friendsOnly
It's the user who defines if the level is unlisted or friend only
*/
void LevelInfos::saveCustomLevelInfos(GJGameLevel* level, bool isUnlisted, bool isFriendOnly, LevelCell* levelCell) {
	log::debug("LevelInfos::saveCustomLevelInfos()");
	if (isUnlisted) {
		levelCell->getChildByID("unlisted-sprite"_spr)->setVisible(true);
		addUnlistedLevel(level);
	}

	if (isFriendOnly) {
		levelCell->getChildByID("friend-only-sprite"_spr)->setVisible(true);
		addFriendOnlyLevel(level);
	}

	levelCell->getChildByID("clock-sprite"_spr)->setVisible(false);
}

void LevelInfos::saveInfosOfLevelsInArray(CCArray* levels) { // fun fact: this comment was written 12 min before new year (2025) in GMT+01&
    for (size_t i = 0; i < levels->count(); i++)
    {
        GJGameLevel* level = typeinfo_cast<GJGameLevel*>(levels->objectAtIndex(i));

        saveLevelInfos(level);
    }
}

// queued levels

auto LevelInfos::getAlreadyQueuedLevels() {
	auto parsedQueuedLevels = matjson::parse(Mod::get()->getSavedValue<std::string>(queuedLevelsKey, EMPTY_ARRAY)); // get the queued levels from the saved key and turn it into a Geode::Result instance
	matjson::Value unwrapedQueuedLevels = parsedQueuedLevels.unwrapOr(EMPTY_ARRAY); // get the queued levels but in a matjson array
	log::info("unwrapedQueuedLevels.size() = {}", unwrapedQueuedLevels.size());
	return unwrapedQueuedLevels;
}

bool LevelInfos::wasAlreadyQueued(GJGameLevel* level) {
	int levelID = level->m_levelID.value();

	matjson::Value queuedLevels = getAlreadyQueuedLevels();

	for (size_t i = 0; i < queuedLevels.size(); i++) // iterate trought each queued only levels
	{
		int queuedLevelID = queuedLevels.get<int>(i).unwrapOr(DEFAULT_NO_LEVEL_ID); // get the level id

		if (levelID == queuedLevelID) {
			return true;
		}
	}

	return false;
}

void LevelInfos::addQueuedLevel(GJGameLevel* level) {
	// if the level is already queued, don't add it to the queued level array
	if (LevelInfos::wasAlreadyQueued(level)) {
		log::info("level {} is already in the '{}' array", level->m_levelName, queuedLevelsKey);
		return;
	}

	matjson::Value queuedLevels = getAlreadyQueuedLevels();
	queuedLevels.push(level->m_levelID.value());

	log::debug("Refreshing the '{}' level array -> size = {}", queuedLevelsKey, queuedLevels.size());
	Mod::get()->setSavedValue<std::string>(queuedLevelsKey, queuedLevels.dump());
}