#include "./levelInfos.hpp"
#include "../utils/utils.hpp"

const std::string EMPTY_ARRAY = matjson::Value::array().dump(matjson::NO_INDENTATION);
const int DEFAULT_NO_LEVEL_ID = 0;

const std::string unlistedLevelsKey = "unlisted-levels";
const std::string friendOnlyLevelsKey = "friend-only-levels";
const std::string queuedLevelsKey = "queued-levels";

void addLevelToQueue(GJGameLevel* level) {
	if (level) {
		LevelInfos::addQueuedLevel(level);
	}
}

void addLevelToQueue(int levelID) {
	LevelInfos::addQueuedLevel(levelID);
}

// unlisted levels


auto LevelInfos::getUnlistedLevels() {
	auto parsedUnlistedLevels = matjson::parse(Mod::get()->getSavedValue<std::string>(unlistedLevelsKey, EMPTY_ARRAY)); // get the unlisted levels from the saved key and turn it into a Geode::Result instance
	matjson::Value unwrapedUnlistedLevels = parsedUnlistedLevels.unwrapOr(EMPTY_ARRAY); // get the unlisted levels but in a matjson array
	return unwrapedUnlistedLevels;
}

bool LevelInfos::isUnlisted(GJGameLevel* level) {
	return isUnlisted(level->m_levelID);
}

bool LevelInfos::isUnlisted(int levelID) {
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
	addUnlistedLevel(level->m_levelID);
}

void LevelInfos::addUnlistedLevel(int levelID) {
	addLevelToQueue(levelID);

	// if the level is already unlisted, don't add it to the unlisted level array
	if (isUnlisted(levelID)) {
		Misc::log_debug(fmt::format("Level with ID {} is already in the '{}' array", levelID, unlistedLevelsKey));
		return;
	}

	Misc::log_debug(fmt::format("Refreshing the '{}' level array", unlistedLevelsKey));

	auto unlistedLevels = getUnlistedLevels();
	unlistedLevels.push(levelID);

	Mod::get()->setSavedValue<std::string>(unlistedLevelsKey, unlistedLevels.dump());
}

void LevelInfos::clearUnlistedLevels() {
	Mod::get()->setSavedValue<std::string>(unlistedLevelsKey, EMPTY_ARRAY);
}

// friend only levels


auto LevelInfos::getFriendOnlyLevels() {
	auto parsedFriendOnlyLevels = matjson::parse(Mod::get()->getSavedValue<std::string>(friendOnlyLevelsKey, EMPTY_ARRAY)); // get the friend only from the saved key and turn it into a Geode::Result instance
	auto unwrapedFriendOnlydLevels = parsedFriendOnlyLevels.unwrapOr(EMPTY_ARRAY); // get the friend only levels but in a matjson array
	return unwrapedFriendOnlydLevels;
}

bool LevelInfos::isFriendOnly(GJGameLevel* level) {
	int levelID = level->m_levelID;

	return isFriendOnly(levelID);
}

bool LevelInfos::isFriendOnly(int levelID) {
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
	addFriendOnlyLevel(level->m_levelID);
}

void LevelInfos::addFriendOnlyLevel(int levelID) {
	addLevelToQueue(levelID);
	
	// if the level is already unlisted, don't add it to the unlisted level array
	if (LevelInfos::isFriendOnly(levelID)) {
		Misc::log_debug(fmt::format("level with ID {} is already in the '{}' array", levelID, friendOnlyLevelsKey));
		return;
	}

	Misc::log_debug(fmt::format("Refreshing the '{}' level array", friendOnlyLevelsKey));

	matjson::Value friendOnlyLevels = getFriendOnlyLevels();
	friendOnlyLevels.push(levelID);

	Mod::get()->setSavedValue<std::string>(friendOnlyLevelsKey, friendOnlyLevels.dump());
}

void LevelInfos::clearFriendOnlyLevels() {
	Mod::get()->setSavedValue<std::string>(friendOnlyLevelsKey, EMPTY_ARRAY);
}

// queued levels


auto LevelInfos::getAlreadyQueuedLevels() {
	auto parsedQueuedLevels = matjson::parse(Mod::get()->getSavedValue<std::string>(queuedLevelsKey, EMPTY_ARRAY)); // get the queued levels from the saved key and turn it into a Geode::Result instance
	matjson::Value unwrapedQueuedLevels = parsedQueuedLevels.unwrapOr(EMPTY_ARRAY); // get the queued levels but in a matjson array
	Misc::log_debug(fmt::format("unwrapedQueuedLevels.size() = {}", unwrapedQueuedLevels.size()));
	return unwrapedQueuedLevels;
}

bool LevelInfos::wasAlreadyQueued(GJGameLevel* level) {
	int levelID = level->m_levelID;

	return wasAlreadyQueued(levelID);
}

bool LevelInfos::wasAlreadyQueued(int levelID) {
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
	addQueuedLevel(level->m_levelID);
}

void LevelInfos::addQueuedLevel(int levelID) {
	// if the level is already queued, don't add it to the queued level array
	if (LevelInfos::wasAlreadyQueued(levelID)) {
		Misc::log_debug(fmt::format("level with ID {} is already in the '{}' array", levelID, queuedLevelsKey));
		return;
	}

	Misc::log_debug(fmt::format("Refreshing the '{}' level array", queuedLevelsKey));

	auto queuedLevels = getAlreadyQueuedLevels();
	queuedLevels.push(levelID);

	Mod::get()->setSavedValue<std::string>(queuedLevelsKey, queuedLevels.dump());
}

void LevelInfos::clearAlreadyQueuedLevels() {
	Mod::get()->setSavedValue<std::string>(queuedLevelsKey, EMPTY_ARRAY);
}

// add the  unlisted / friend only  levels


void LevelInfos::saveLevelInfos(GJGameLevel* level) {
	LevelInfos::addQueuedLevel(level);

	if (level->m_unlisted) {
		Misc::log_debug("Level is unlisted");
		addUnlistedLevel(level);
	}

	if (level->m_friendsOnly) {
		Misc::log_debug("Level is friend only");
		addFriendOnlyLevel(level);
	}
}

/*
This function is like saveLevelInfos except it won't see the content of level->m_unlisted and level->m_friendsOnly
It's the mod that defines if the level is unlisted or friend only
*/
void LevelInfos::saveCustomLevelInfos(GJGameLevel* level, bool isUnlisted, bool isFriendOnly) {
	Misc::log_debug("LevelInfos::saveCustomLevelInfos()");
	if (isUnlisted) {
		addUnlistedLevel(level);
	}

	if (isFriendOnly) {
		addFriendOnlyLevel(level);
	}
}

/*
This function is like saveLevelInfos except it won't see the content of level->m_unlisted and level->m_friendsOnly
It's the mod that defines if the level is unlisted or friend only
*/
void LevelInfos::saveCustomLevelInfos(int levelID, bool isUnlisted, bool isFriendOnly) {
	Misc::log_debug("LevelInfos::saveCustomLevelInfos()");
	if (isUnlisted) {
		addUnlistedLevel(levelID);
	}

	if (isFriendOnly) {
		addFriendOnlyLevel(levelID);
	}
}

/*
This function is like saveLevelInfos except it won't see the content of level->m_unlisted and level->m_friendsOnly
It's the mod that defines if the level is unlisted or friend only
*/
void LevelInfos::saveCustomLevelInfos(LevelCell* levelCell, bool isUnlisted, bool isFriendOnly) {
	Misc::log_debug("LevelInfos::saveCustomLevelInfos()");
	if (isUnlisted) {
		Ref<CCFadeTo> iconFades = CCFadeTo::create(Fades::Fades::iconsFadeInTime, Fades::FadeTo::iconsFadeTo);
		Ref<CCSprite> unlistedSprite = typeinfo_cast<CCSprite*>(levelCell->getChildByID(Ids::UNLISTED_SPRITE_ID));

		unlistedSprite->setOpacity(0);
		unlistedSprite->setVisible(true);
		unlistedSprite->runAction(iconFades);

		addUnlistedLevel(levelCell->m_level);
	}

	if (isFriendOnly) {
		Ref<CCFadeTo> FadeTo = CCFadeTo::create(Fades::Fades::iconsFadeInTime, Fades::FadeTo::iconsFadeTo);
		Ref<CCSprite> friendOnlySprite = typeinfo_cast<CCSprite*>(levelCell->getChildByID(Ids::FRIEND_ONLY_SPRITE_ID));

		friendOnlySprite->setVisible(true);
		friendOnlySprite->setOpacity(0);
		friendOnlySprite->runAction(FadeTo);

		addFriendOnlyLevel(levelCell->m_level);
	}
}

void LevelInfos::saveInfosOfLevelsInArray(CCArray* levels) { // fun fact: this comment was written 12 min before new year (2025) in GMT+01&
    for (size_t i = 0; i < levels->count(); i++)
    {
        GJGameLevel* level = typeinfo_cast<GJGameLevel*>(levels->objectAtIndex(i));

        saveLevelInfos(level);
    }
}


// other stuff


bool LevelInfos::isLevelDaily(GJGameLevel* level) {
	return (level->m_dailyID.value() != 0);
}