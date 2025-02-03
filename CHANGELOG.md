# NetPanzer Changelog

## 0.9.0 - "For Fu"

0.9.0 brings a lot of changes over 0.8, and is the first release of NetPanzer since 2017. It is dedicated to our friend Fu,
and was completed with the help from Andy (andy5995), Jammy (Jammyjamjamman), and Phelipe (ph2708). 

## New Features
* Game Background and UI Updated by @winrid and @ph2708  in https://github.com/netpanzer/netpanzer/pull/98
* Added Map: Arena Fut by @andy5995 in https://github.com/netpanzer/netpanzer/pull/111

## Bugs Fixed
* Potential crash when deleting/editing text in text input fields. by @andy5995 
* Fix Game Config Not Saving by @winrid in https://github.com/netpanzer/netpanzer/pull/86
* Caching Introduced to Font Rendering to reduce potential frame rate drops by @winrid in https://github.com/netpanzer/netpanzer/pull/94
* Improving fullscreen switch reliability by @winrid in https://github.com/netpanzer/netpanzer/pull/110
* Fixes mouse zooming to top left by @winrid in https://github.com/netpanzer/netpanzer/pull/113
* Mouse Movement Fixes (Jerky Movement etc) by @winrid in https://github.com/netpanzer/netpanzer/pull/121
* Fixes server using player.name instead of server.name config for dedicated servers by @winrid in https://github.com/netpanzer/netpanzer/pull/139

## Other Changes
* Switching to the Meson build system. by @andy5995 
* Documentation improvements. by @andy5995 @winrid 
* Can now compile for MacOS by @andy5995 
* Can now build and run server from Docker by @andy5995 
* Updated Credits by @andy5995 in https://github.com/netpanzer/netpanzer/pull/107
* Introducing new "data dir" concept by @andy5995 


* meson: use wraps and fallbacks for some dependencies by @andy5995 in https://github.com/netpanzer/netpanzer/pull/133
* Build(deps): Bump actions/checkout from 2 to 4 by @dependabot in https://github.com/netpanzer/netpanzer/pull/151
* Fix some shadow declarations and CodeQL warning by @andy5995 in https://github.com/netpanzer/netpanzer/pull/156
* Fix crashing at main menu when sanitize is enabled by @andy5995 in https://github.com/netpanzer/netpanzer/pull/157
* maint: Remove Lib/INIParser by @andy5995 in https://github.com/netpanzer/netpanzer/pull/158
* meson: Post release version bump to 0.9.0.07.555 by @andy5995 in https://github.com/netpanzer/netpanzer/pull/160
* CI: Edit trigger filters by @andy5995 in https://github.com/netpanzer/netpanzer/pull/154
* docs: Add WhatsApp and Instagram group by @andy5995 in https://github.com/netpanzer/netpanzer/pull/153
* Fixing Various Segfaults and Warnings by @winrid in https://github.com/netpanzer/netpanzer/pull/169
* Add "starting soon" message to winner screen by @andy5995 in https://github.com/netpanzer/netpanzer/pull/168
* Move data by @andy5995 in https://github.com/netpanzer/netpanzer/pull/170
* Implement localization by @andy5995 in https://github.com/netpanzer/netpanzer/pull/155
* Fixes Random Crash by @winrid in https://github.com/netpanzer/netpanzer/pull/187
* Adding Proximity Audio For Hits and Explosions, Fixes Variable Audio Broken by @winrid in https://github.com/netpanzer/netpanzer/pull/188
* CI: Copy filters to other 2 tasks (fix) by @andy5995 in https://github.com/netpanzer/netpanzer/pull/190
* Build(deps): Bump RafikFarhad/clang-format-github-action from 3 to 4 by @dependabot in https://github.com/netpanzer/netpanzer/pull/196
* maint: Patch netpanzer.desktop by @andy5995 in https://github.com/netpanzer/netpanzer/pull/195
* docs,scripts: Update for use of subprojects, fix data install by @andy5995 in https://github.com/netpanzer/netpanzer/pull/194
* Add pak tools to build system by @andy5995 in https://github.com/netpanzer/netpanzer/pull/198
* Make game_config cli option work as expected by @andy5995 in https://github.com/netpanzer/netpanzer/pull/201
* CI-AppImage: Build arm64 image by @andy5995 in https://github.com/netpanzer/netpanzer/pull/197
* tests: Enable on Windows by @andy5995 in https://github.com/netpanzer/netpanzer/pull/183
* nls: Disable on Windows by @andy5995 in https://github.com/netpanzer/netpanzer/pull/204
* maint: Fix some PHYSFS deprecations warnings by @andy5995 in https://github.com/netpanzer/netpanzer/pull/209
* maint: Narrow scope of cleanupThreshold by @andy5995 in https://github.com/netpanzer/netpanzer/pull/207
* tests: Add test for netpanzer when '--help' is used by @andy5995 in https://github.com/netpanzer/netpanzer/pull/208
* meson: Add option - build_tests by @andy5995 in https://github.com/netpanzer/netpanzer/pull/210
* Improving Right Mouse Movement + Removing Black Bars by @winrid in https://github.com/netpanzer/netpanzer/pull/203
* Support Variable Width Text in Top Buttons by @winrid in https://github.com/netpanzer/netpanzer/pull/214
* CI: Use filter template (yaml alias) for Cirrus by @andy5995 in https://github.com/netpanzer/netpanzer/pull/212
* appimage: Use linuxdeploy-build-helper-container (remove focal build-env) by @andy5995 in https://github.com/netpanzer/netpanzer/pull/202
* CI: Update image:tag for arm64 container in cirrus by @andy5995 in https://github.com/netpanzer/netpanzer/pull/219
* CI: Improvments, remove cache by @andy5995 in https://github.com/netpanzer/netpanzer/pull/221
* meson: Write the correct path to the data in config.h by @andy5995 in https://github.com/netpanzer/netpanzer/pull/220
* Docker server: Switch to GitHub container registry (ghcr.io) by @andy5995 in https://github.com/netpanzer/netpanzer/pull/222
* Fix Sanitizer Warnings in Netcode by @winrid in https://github.com/netpanzer/netpanzer/pull/223
* meson: Remove ogg and vorbis by @andy5995 in https://github.com/netpanzer/netpanzer/pull/224
* Start defining properties for menu in lua script by @Jammyjamjamman in https://github.com/netpanzer/netpanzer/pull/200
* Fix color by @Jammyjamjamman in https://github.com/netpanzer/netpanzer/pull/225
* meson: Use alpha chars in version number by @andy5995 in https://github.com/netpanzer/netpanzer/pull/226
* CI: Bump FreeBSD to 14.0 by @andy5995 in https://github.com/netpanzer/netpanzer/pull/227
* Make Text Smooth by @winrid in https://github.com/netpanzer/netpanzer/pull/228
* Add get_version.py by @andy5995 in https://github.com/netpanzer/netpanzer/pull/229
* Wrapped Text Support With Variable Speed Scrolling by @winrid in https://github.com/netpanzer/netpanzer/pull/230
* Fix OSX Build by @winrid in https://github.com/netpanzer/netpanzer/pull/234
* Update Credits by @winrid in https://github.com/netpanzer/netpanzer/pull/233
* Build(deps): Bump docker/build-push-action from 5 to 6 by @dependabot in https://github.com/netpanzer/netpanzer/pull/231
* CI: Replace deprecated macos with sonoma by @andy5995 in https://github.com/netpanzer/netpanzer/pull/236
* Set required versions for SDL2 and SDL2_ttf, add Ubuntu Focal test to CI by @andy5995 in https://github.com/netpanzer/netpanzer/pull/237
* Fix code scanning alert #36: Multiplication result converted to larger type by @andy5995 in https://github.com/netpanzer/netpanzer/pull/238
* Fix code scanning alert #9: Comparison of narrow type with wide type in loop condition by @andy5995 in https://github.com/netpanzer/netpanzer/pull/239
* nls: Mark more strings and add translations by @andy5995 in https://github.com/netpanzer/netpanzer/pull/193
* Server Crash Fixes & Sanitizer Warnings by @winrid in https://github.com/netpanzer/netpanzer/pull/241
* Fix code-scanning alerts (wrong type of arguments to formatting funct… by @andy5995 in https://github.com/netpanzer/netpanzer/pull/243
* Add cross-compiling support by @andy5995 in https://github.com/netpanzer/netpanzer/pull/244
* CI: Add Ubuntu 24.04 (Noble) by @andy5995 in https://github.com/netpanzer/netpanzer/pull/247
* docs: Update for docker by @andy5995 in https://github.com/netpanzer/netpanzer/pull/246
* meson: Install docs (closes #140) by @andy5995 in https://github.com/netpanzer/netpanzer/pull/248
* Fix code scanning alert no. 35: Multiplication result converted to larger type by @andy5995 in https://github.com/netpanzer/netpanzer/pull/242
* Adding Bounds Check for Heap Buffer Overflow by @winrid in https://github.com/netpanzer/netpanzer/pull/251
* CI: refactor matrix by @andy5995 in https://github.com/netpanzer/netpanzer/pull/249
* Add docs from meson install to Dockerfile (fix) by @andy5995 in https://github.com/netpanzer/netpanzer/pull/253
* Add coverity.yml by @andy5995 in https://github.com/netpanzer/netpanzer/pull/254
* appimage.yml: Use andy5995/linuxdeploy:v2-focal instead of '...:latest' by @andy5995 in https://github.com/netpanzer/netpanzer/pull/256
* CI: Fix coverity scan, use EMAIL by @andy5995 in https://github.com/netpanzer/netpanzer/pull/255
* maint: Fix some coverity warnings by @andy5995 in https://github.com/netpanzer/netpanzer/pull/259
* Add test for lto and extra Werror flags by @andy5995 in https://github.com/netpanzer/netpanzer/pull/258
* Remove unused docs from root directory by @andy5995 in https://github.com/netpanzer/netpanzer/pull/260
* CI: Bump FreeBSD to 14.1 by @andy5995 in https://github.com/netpanzer/netpanzer/pull/261
* Fix Bot Crash in Dev Env by @winrid in https://github.com/netpanzer/netpanzer/pull/263
* Fixing volume controls not affecting ambient sounds, explosions, etc. by @winrid in https://github.com/netpanzer/netpanzer/pull/266
* Fixes Bots Crashing - Always use physfs by @winrid in https://github.com/netpanzer/netpanzer/pull/268

### Bugs Fixed

- Fixed flag alignment in chat console (due to font change).
- Fixed flag alignment next to players (due to font change).
- UI and Fonts less blurry when game resolution does not match desktop resolution.
- Fixes mouse getting stuck after changing game resolution.
- Fixes crash sometimes when switching from fullscreen to windowed and back again.
- Fixes freezing when changing screen resolution or windowed/fullscreen mode.
- Fixed alignment with Ranking UI.
- Fixed alignment of arrows around unit style selection.

### New Features

- Background image change.
- Anti Cheat Improvements - see README for new setting. The `game.anticheat` setting has been replaced.
### Bugs Fixed

1. Fixed not being able to use arrow keys to navigate map if mouse is inside minimap.
2. Fixed crashing if one master server is not available when hosting a game.
3. Fixes for a crash during resolution selection when list is too large to fit on screen.

### New Features

1. Updated server list to hide old incompatible servers. It still shows *newer* incompatible servers to encourage upgrading.
2. Added ability to disable interactive console to run server as daemon.
3. Fancy New font!
4. Bots now pick a random flag instead of a blank image.

### Other Improvements

1. Fixed c++17 compatibility (errors, some warnings). Build targets c++17 now.
2. Network layer improvements (fixed catching some exceptions by-value instead of by-ref, fixes some potential memory issues reported by valgrind).
3. Added netpanzer.io to master server list.
5. SConstruct/build system migrated to Python 3.
6. Able to compile on windows with MingW
7. Support for rendering TTF fonts! This was a major refactor.
8. Improved support for other languages than just English (more characters supported now).
9. Migrated to SDL2 w/ new rendering mechanism that can scale the game regardless of resolution (experimental).

### Not Final Release?

There are still some things to address before we release 0.9.0:

1. Font rendering needs to be optimized more. You might see some frame rate drops, this might be why.
2. We need some more play testing to see if the new rendering system is stable.
3. There are some issues regarding anti cheat that need to be improved.

-------------------------------------------------------------




The following is a list of the main FIXES/NEW FEATURES in this Development Release:


  XXX RC4 XXX


  - new extra top info bar with live ping stats to and from gameserver (toggle it with F7)

  - newly refactored bots - 2 types now:
    bot class = 1 (aka the 'Zombie' Bot) is the original bot revisited (much better shooting and after taking max bases allowed it attacks the enemies)
    bot_class = 2 (aka the 'Fighter' Bot) is the new Bot for duels. He follows you through the map...
    for each bot you can choose bot_allied = true or false -> that bot will ask to ally to other bots in game...
    for each bot you can set action_speed = [from 1 to 7] - this is the power level of bots...
    (a third bot type who conquer outposts while talking about politics and religion is in preparation... hehe)

  - the new ASTOUNDING 'Desert' tileset/mapstyle by Silvestre (expect more surprises from him soon) !

  - a new 'mapstyle' button in Host panel so you can change mapstyle locally without editing config files. Good for testing mapstyles.

  - network scrambling is now default, if for some reason you want to deactivate it you can run the server in DEBUG mode (option -g). 

  - added icons in ranking TAB that show if player is human or bot (must add a captcha soon..) - also the 'mute' icons have been replaced.
    


  ... and many changes 'under the hood' like the elimination of some memory leaks (both on runtime and on exit), correction of some bugs on restart and other stuff in preparation of new features.



  P.S. Next RC could be the last trial before official release...





-----------------------------



  FROM RC3



- While re-writing the encryption part in RC2 (btw now it actually works), I ended up disabling the anti-spam system... now it's back to its place.

- I totally re-factored the unit style selection system. It worked fine, but it was highy inefficient both in terms of network and memory.
  Now it is optimized. You will probably notice that the connection time is shorter...

- Added 'Abandon' button in base unit selection window - now you can disown outposts. Note: 'This' means just that outpost,
  while 'All' means all of your outposts.

- In the global destruct powerup (a red one), the unlucky player now loses all of his outposts too.


- fixed visualization of names in outposts (owners, outpost names, units in production etc). Now they are responsive based on string length. 
 

- I deleted a timer in the input management - it could have been an advantage for those unfair player who use tools to bypass it...
  You might notice a slightly better reaction time for movements and shooting.

- I applied a (actually stupid) patch in the pathfinding code and it seems that the negative effect of pathfinding panic (the infamous 'node unreachable' messages in server)
  with tanks slowing down and blocking are now HIGHLY REDUCED! Not tested much, but if it's gonna be true you guys will have to pay me a beer to say the least ;-)  



-----------------------------



  FROM RC2:

- Ok admittedly the scrambler didn't work as expected in RC1. This time should work correctly.
  If it is so, this might be THE END OF CHEATING (no more Cheat Engine, packet editors etc.) as we know it in netpanzer. Let's wait and see...

- The flag change network message has been added to the anti-spam list (no more pseudo-animated flags)

- Added a 'mute' button in Ranking Tab (an 'M' at the end of each line). Somebody is saying terrible things about you in game? Don't get angry, just MUTE him!
  You will not hear anything from him anymore (including chat, ally chat, ally chat requests)! :D   

- Powerups (there were many suggestions). I decided to add many other global unit powerups - which affect ALL player's units.
  As they are more powerful they also are more rare (the powerup thunderbolt is RED in this case - btw made by Silvestre).
  This is the probability table now: 50% single units powerups, 30% bonus units powerups, 20% global unit powerups.
  The global unit powerups are: global repair, global speed +20%, global firepower +20%, global reload -20%, global destruct

- Added mapstyle 'Desert' - a beautiful artwork by Silvestre.



-------------------------



  FROM RC1:

  
- multi unit styles (the famous set by Nessie+others made by me)

  unit styles are chosen once connected to a gameserver. A new window appears. You can toggle that window with 'u' key. By clicking on a unit style you're redirected to flags window.
  Until you have chosen a flag you can always go back and change your selection. For example you might want to see what style of unit the other players have chosen before selecting yours.
  The styles available in gameserver are set in server.cfg  


- multi map styles (quick/cheap but funny color filtering of our only tileset)

  mapstyle (that you can set in server.cfg) can be one of the following: SummerDay (the original one), IcyWinter, Moonlight, Scorched and Martian. 
  
  
- powerups refactored

  unit properties can finally change dinamically and are sinced through the network - remember those powerups like "unit speed powerup" that never worked? Well now are fully operational.   
  the new bonuses are: unit speed powerup (50% faster every hit), firepower, reload, weapon range, superunit (a unit with the best of all other units) and global repair (all your units get repaired instantly)   


- LAST MINUTE FIX! I just noticed that after a restart powerups don't appear anymore on clients screens (if they were already connected before). And that was a pity now that powerups finally work!
  The problem seems to affect previous versions too. So there is now a fix (but has to be verified) - and in general the 'restarting' phase needs more coding work... 


- bots are back!

  did you miss them? Hehe! It's now actually possible to write much more 'smart' bots as the client (or the Bot) has got all the information synced with gameserver.
  My bet is that the most fun will soon come from AI here in netpanzer! You can allow bots in your gameserver in server.cfg.


- new game modes

  game mode 3 (objective AND frag limit) - you must have both to win; game mode 4 (frag limit OR time limit) - whichever happens first.
  the objective percentage, frag limit and time are shown correctly in the top game bar. 


- new respawn type

  respawtype = 2 (random alternative) - It's random but never 2 consecutive respawns in the same spot.


- network data 'scrambled'

  not exactly an ecryption, as the cipher adopted here is trivial and only some bytes are encoded but...
  since a new key is generated by gameserver every 30 secs a random data stream is created and IMHO this could be enough to make most packet sniffers/editors around ineffective.
  (after all we are targeting lamers)


- player names sanitized

  finally no more 'Joe' and 'Joe ' that appear both in game as 'Joe'! Player names are trimmed, multiple spaces and non printable characters eliminated.
  chars allowed are alphanumeric + a subset of ASCII printable chars (99% of already registered nicknames should be ok).
    

- SHIFT + m now toggles a bigger map.


- Radar is now always on. Since it was trivial to make it so by changing a couple of lines in the source... 





And many others minor undocumented changes and fixes.

A better list will be added on official release. 



The new unit styles are: original, danisch, desert, metro, night, nva, platane, surpat, tiger, woodland, nva2, platane2, scorched, martian

The new tileset styles (or mapstyle) are (note the capital letters): SummerDay, IcyWinter, Moonlight, Scorched, Martian

New in server.cfg: 

- game.bots_allowed = false (or true)
- game.mapstyle = "[string]"

game.gametype can now be also 3 (objective AND frag limit) or 4 (fraglimit OR timelimit)

game.respawntype can now be also 2 (random alt) 




-------------------------------------------------------------




