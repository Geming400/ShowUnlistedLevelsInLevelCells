#include "level/LevelInfos.hpp"
#include "level/levelClass.hpp"
#include "queue/QueueRequests.hpp"
#include "utils/utils.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
// The layers under this comment will be used to pick up new unlisted levels
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>

using namespace geode::prelude;

const CCPoint firstPos = {340, 10}; // the first pos, aka it's on the left side
const CCPoint secondPos = {325, 10}; // the second pos, aka it's on the left side but a little bit less

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
AND
if the level is a daily / weekly / event  level
*/
bool wasQueued(GJGameLevel* level) {
	return (LevelInfos::wasAlreadyQueued(level) && QueueRequests::get()->isQueued(level) && QueueRequests::get()->isQueuedInTempQueue(level) && LevelInfos::isLevelDaily(level));
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
	gets the n-th parent of 'this', then typeinfo casts it into T

	@param n the num of iterations to get the desired parent
	*/
	template<typename T>
	T getRecursiveParent(int n) {
		CCNode* parent = this->getParent();

		if (parent && n <= 0) {
			return typeinfo_cast<T>(parent);
		} else if (parent && n > 0) {
			return typeinfo_cast<T>(getRecursiveParent(n-1));
		}
		return nullptr;
	}
	/*gets the n-th parent of 'this', then returns a `CCNode` pointer*/
	CCNode* getRecursiveParent(int n) {
		CCNode* parent = this->getParent();

		if (parent && n <= 0) {
			return parent;
		} else if (parent && n > 0) {
			return getRecursiveParent(n-1);
		}
		return nullptr;
	}

	/*
	This will get the search type from a GJSearchObject from the fifth parent (yes I could have hooked LevelBrowserLayer but whatever)
	*/
	bool isSearchTypeAllowed() {
		if (LevelInfos::isLevelDaily(m_level)) {
			log::info("Search type for daily levels is not allowed");
			return false;
		}
		return true;
		
		/*log::debug("hello 0.1");
		//LevelBrowserLayer* levelBrowserLayer = typeinfo_cast<LevelBrowserLayer*>(this->getParent()
		//->getParent()
		//->getParent()
		//->getParent()
		//->getParent());

		LevelBrowserLayer* levelBrowserLayer = getRecursiveParent<LevelBrowserLayer*>(5); // get the fifth parent

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
		return false;*/
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

		m_fields->m_unlistedSprite->setID(Ids::UNLISTED_SPRITE_ID);
		m_fields->m_unlistedSprite->setPosition(firstPos);
		m_fields->m_unlistedSprite->setScale(0.5f);
		m_fields->m_unlistedSprite->setOpacity(Fades::FadeTo::iconsFadeTo);
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

		m_fields->m_friendOnlySprite->setID(Ids::FRIEND_ONLY_SPRITE_ID);
		if (Mod::get()->getSettingValue<bool>("show-unlisted-sprite")) {
			m_fields->m_friendOnlySprite->setPosition(secondPos);
		} else {
			m_fields->m_friendOnlySprite->setPosition(firstPos);
		}
		m_fields->m_friendOnlySprite->setScale(0.5f);
		m_fields->m_friendOnlySprite->setOpacity(Fades::FadeTo::iconsFadeTo);
		m_fields->m_friendOnlySprite->setVisible(false);
		
		
		addChild(m_fields->m_friendOnlySprite);

		if (m_level->m_friendsOnly || m_fields->m_isFriendOnly && !wasQueued(m_level)) { // '!wasQueued(m_level)' is a extra check to prevent a bug where the level has
			if (Mod::get()->getSettingValue<bool>("show-friend-only-sprite")) {			 // the clock icon visible and the unlisted icon / friends only icon too
				m_fields->m_friendOnlySprite->setVisible(true);
			}
		}

		m_fields->m_clockSprite = CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png");

		m_fields->m_clockSprite->setID(Ids::CLOCK_SPRITE_ID);
		m_fields->m_clockSprite->setPosition(firstPos);
		m_fields->m_clockSprite->setScale(0.5f);
		m_fields->m_clockSprite->setOpacity(Fades::FadeTo::iconsFadeTo);
		m_fields->m_clockSprite->setVisible(false);
		

		addChild(m_fields->m_clockSprite);

		// Adding the level in queue or something

		if (!(m_fields->m_isUnlisted && m_fields->m_isFriendOnly) && !wasQueued(m_level)) {
			log::debug("Adding {} to queue from main.cpp", m_level->m_levelName);
			QueueRequests::get()->addLevelToQueue(this);
		}

		// showing the clock sprite

		if (!(m_fields->m_isUnlisted && m_fields->m_isFriendOnly) && !wasQueued(m_level)) {
			if (Mod::get()->getSettingValue<bool>("show-clock-icon") && Mod::get()->getSettingValue<bool>("queue-requests") && QueueRequests::get()->isQueued(m_level)) {
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

		QueueRequests::get()->startLoop();

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
				LevelInfos::saveLevelInfos(level); // save it's info the (unlisted || friendsOnly) is already in LevelInfos::saveLevelInfos()
			}
			
			LevelInfos::addQueuedLevel(level); // prevent the level from being queued
		}

		localLevelsHasBeenChecked(); // make it so that this will not happen again
		return true;
	}
};

class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {
	bool init(GJSearchObject* p0) {
		log::debug("LevelBrowserLayer::init()");
		if (!LevelBrowserLayer::init(p0)) return false;
		
		CCArray* entries = CCArray::create();

		if (m_list->m_listView) {
			entries = m_list->m_listView->m_entries;
			log::info("entries is not nullptr !");
		} else {
			log::warn("Whar. entries == nullptr");
		}

		for (size_t i = 0; i < entries->count(); i++)
		{
			auto element = typeinfo_cast<LevelCell*>(entries->objectAtIndex(i));
			if (element) {
				LevelCells::updateLevelCell(element);
			}
		}
		
		return true;
	}
};