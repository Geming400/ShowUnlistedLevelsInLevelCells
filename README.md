# ShowUnlistedLevelsInLevelCells

This is where she makes a mod.

<img src="logo.png" width="150" alt="the mod's logo"/>

---

Show unlisted levels in level cells is a mod that will show which level is unlisted or not:

- The unlisted icon: 

<img src="./resources/unlisted-icon.png" alt="the unlisted icon">

- The friend only icon: 

<img src="./resources/friend-only-icon.png" alt="the friend only icon">

## How does this mod work ?

### on first launch

On first launch, this mod will check for every local levels you have and see if they are unlisted, friends only or not.

### on level upload

When uploading a level this mod will check if the uploaded level is unlisted or friends only or not.

### when discovering levels

When discovering levels *(searching for levels or going to your saved levels)* this mod will put in a queue the level and check if the level is friends only. (At the moment, the mod doesn't check if the mod is simply unlisted).

When the level is in queue a clock icon will show up (it's disabled by default in the mod's settings) telling the level is in queue

## Contributing

I tried making a lot of comments to make my code understandable. I'm not the best at c++ so sorry for that in advance.
If you have any issue understanding what I did, be sure to dm on discord (linked on my github socials) or make an issue. I don't mind !

## TODO

- ~~Use `m_tempQueuedLevelList`~~
- Use `CCArray`s when possible instead of `std::vector`s.
- Make a mod icon
- Maybe fix android build issues
- Make that requests are qued based on their ids or something
- Fix the previews' images inconcistency