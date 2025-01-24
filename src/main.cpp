#include "levelInfos.hpp"
#include "queueRequests.hpp"
#include "levelClass.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/MenuLayer.hpp>
// The layers under this comment will be used to pick up new unlisted levels
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>

using namespace geode::prelude;

const CCPoint firstPos = {340, 10}; // the first pos, aka it's on the left side
const CCPoint secondPos = {325, 10}; // the second pos, aka it's on the left side but a little bit less

QueueRequests queueRequests;

const std::vector<SearchType> allowedTypes = {
	SearchType::Downloaded,
	SearchType::FavouriteLevels,
	SearchType::MyLevels,
	// SearchType::Reported, // wth is this
	SearchType::SavedLevels,
	SearchType::Search
};

/* 
if the level was already queued
AND
if the level is currently in the queue
AND
if the level is currently in the temp queue
*/
bool wasQueued(GJGameLevel* level) {
	return (LevelInfos::wasAlreadyQueued(level) && queueRequests.isQueued(level) && queueRequests.isQueuedInTempQueue(level));
}

class $modify(MyEditLevelLayer, EditLevelLayer) {
	bool init(GJGameLevel *p0) {
		if (!EditLevelLayer::init(p0)) {
			return false;
		}

		if (m_level) {
			LevelInfos::saveLevelInfos(m_level);
		}

		return true;
	}

	void uploadActionFinished(int p0, int p1) { // the level just finished uploading, meaning we can now checked if it's unlisted or not
		EditLevelLayer::uploadActionFinished(p0, p1);
		LevelInfos::saveLevelInfos(m_level);
	}
};

class $modify(MyLevelCell, LevelCell) {
	struct Fields {
		bool m_isUnlisted;
		bool m_isFriendOnly;

		CCSprite* m_unlistedSprite;
		CCSprite* m_friendOnlySprite;
		CCSprite* m_clockSprite;
	};

	bool isSearchTypeAllowed(SearchType searchType) {
		for (size_t i = 0; i < allowedTypes.size(); i++)
		{
			if (searchType == allowedTypes.at(i)) {
				log::info("Search type {} is allowed", static_cast<int>(searchType));
				return true;
			}
		}
		log::info("Search type {} is NOT allowed", static_cast<int>(searchType));
		return false;
	}

	/*
	This will get the search type from a GJSearchObject from the fifth parent (yes I could have hooked LevelBrowserLayer but whatever)
	*/
	bool isSearchTypeAllowed() {
		return true;
		if (typeinfo_cast<DailyLevelNode*>(this->getParent())) {
			log::info("Search type for daily levels is not allowed");
			return false; // the user is on the  daily / weekly / event  level thingy
		} else {
			log::info("typeinfo_cast<DailyLevelNode*>(this->getParent()) = nullptr");
			if (this->getParent()) {
				log::debug("this->getParent() = True");
			} else {
				log::debug("this->getParent() = False");
			}
			log::debug("this->getID() = {}", this->getParent()->getID());
		}
		return false;
		log::debug("hello 0.1");
		LevelBrowserLayer* levelBrowserLayer = typeinfo_cast<LevelBrowserLayer*>(this->getParent()
		->getParent()
		->getParent()
		->getParent()
		->getParent());
		log::debug("hello 0.2");
		
		if (levelBrowserLayer) {
			log::debug("hello 1");
			GJSearchObject* searchObject = levelBrowserLayer->m_searchObject;
			SearchType searchType = searchObject->m_searchType;
			log::debug("hello 1.5");
			for (size_t i = 0; i < allowedTypes.size(); i++)
			{
				log::debug("hello 2");
				if (searchType == allowedTypes.at(i)) {
					log::info("Search type {} is allowed", static_cast<int>(searchType));
					return true;
				}
			}
			log::info("Search type {} is NOT allowed or is not in 'allowedTypes'", static_cast<int>(searchType));
		} else {
			log::info("not if (levelBrowserLayer)");
		}
		return false;
	}


    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();
		if (!isSearchTypeAllowed()) {
			return;
		}

		m_fields->m_isUnlisted = LevelInfos::isUnlisted(m_level);
		m_fields->m_isFriendOnly = LevelInfos::isFriendOnly(m_level);



		// unlistedSprite

		m_fields->m_unlistedSprite = CCSprite::create("unlisted-icon.png"_spr);

		m_fields->m_unlistedSprite->setID("unlisted-sprite"_spr);
		m_fields->m_unlistedSprite->setPosition(firstPos);
		m_fields->m_unlistedSprite->setScale(0.5f);
		m_fields->m_unlistedSprite->setOpacity(128);
		m_fields->m_unlistedSprite->setVisible(false);

		addChild(m_fields->m_unlistedSprite);

		if (m_level->m_unlisted || m_fields->m_isUnlisted) {
			if (Mod::get()->getSettingValue<bool>("show-unlisted-sprite"))
			{
				m_fields->m_unlistedSprite->setVisible(true);
			}
		}



		// friendOnlySprite

		m_fields->m_friendOnlySprite = CCSprite::create("friend-only-icon.png"_spr);

		m_fields->m_friendOnlySprite->setID("friend-only-sprite"_spr);
		if (Mod::get()->getSettingValue<bool>("show-unlisted-sprite")) {
			m_fields->m_friendOnlySprite->setPosition(secondPos);
		} else {
			m_fields->m_friendOnlySprite->setPosition(firstPos);
		}
		m_fields->m_friendOnlySprite->setScale(0.5f);
		m_fields->m_friendOnlySprite->setOpacity(128);
		m_fields->m_friendOnlySprite->setVisible(false);

		addChild(m_fields->m_friendOnlySprite);

		if (m_level->m_friendsOnly || m_fields->m_isFriendOnly) {
			if (Mod::get()->getSettingValue<bool>("show-friend-only-sprite"))
			{
				m_fields->m_friendOnlySprite->setVisible(true);
			}
		}

		m_fields->m_clockSprite = CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png");

		m_fields->m_clockSprite->setID("clock-sprite"_spr);
		m_fields->m_clockSprite->setPosition(firstPos);
		m_fields->m_clockSprite->setScale(0.5f);
		m_fields->m_clockSprite->setOpacity(128);
		m_fields->m_clockSprite->setVisible(false);

		addChild(m_fields->m_clockSprite);

		// Adding the level in queue or something

		if (!(m_fields->m_isUnlisted && m_fields->m_isFriendOnly) && !wasQueued(m_level)) {
			log::debug("Adding {} to queue from main.cpp", m_level->m_levelName);
			LevelClass levelClass;
			levelClass.level = m_level;
			levelClass.levelCell = this;
			levelClass.clockSprite = m_fields->m_clockSprite;
			queueRequests.addLevelToQueue(levelClass);
		}

		// showing the clock sprite

		if (!(m_fields->m_isUnlisted && m_fields->m_isFriendOnly)) {
			if (Mod::get()->getSettingValue<bool>("show-clock-icon") && Mod::get()->getSettingValue<bool>("queue-requests") && queueRequests.isQueued(m_level)) {
				m_fields->m_clockSprite->setVisible(true);
			}
		}
    }
};

// getting all of the local levels to check if they are unlisted or not
class $modify(MyMenuLayer, MenuLayer) {
	bool hasLocalLevelsBeenChecked() {
		return Mod::get()->getSavedValue<bool>("has-local-levels-been-checked", false);
	}

	void localLevelsHasBeenChecked() {
		Mod::get()->setSavedValue("has-local-levels-been-checked", true);
	}

	bool init() {
		if (!MenuLayer::init()) {
			return false;
		}

		queueRequests.startLoop();

		if (hasLocalLevelsBeenChecked()) {
			log::info("Local levels have already been checked");
			return true;
		}

		LocalLevelManager* localLevelManager = LocalLevelManager::sharedState();
		cocos2d::CCArray* localLevels = localLevelManager->m_localLevels;

		log::debug("First time loading:");
		for (size_t i = 0; i < localLevels->count(); i++) // iterate trought every local levels
		{
			GJGameLevel* level = typeinfo_cast<GJGameLevel*>(localLevels->objectAtIndex(i)); // cast the CCObject* into a GJGameLevel*

			if (level->m_unlisted || level->m_friendsOnly) { // if the level is either unlisted or friend only
				log::debug("Level {} will have it's info saved", level->m_levelName);
			}

			LevelInfos::saveLevelInfos(level); // save it's info the (unlisted || friendsOnly) is already in LevelInfos::saveLevelInfos()
		}

		localLevelsHasBeenChecked(); // make it so that this will not happen again
		return true;
	}
};