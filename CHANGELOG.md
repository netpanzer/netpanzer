# NetPanzer Changelog

## 0.9.0 - "Fulvio"

0.9.0 brings a lot of changes over 0.8, and is the first release of NetPanzer since 2017. It is dedicated to our friend Fu who passed away in 2024,
and was completed with the help from Andy (andy5995), Jammy (Jammyjamjamman), and Phelipe (ph2708).

Fu did a lot of work toward the 0.9.x release. Before his passing he mentioned several things he wanted to see, like migrating to SDL2, support for TTF fonts, and so on. We
made sure to incorporate these improvements in the release. I was the only person working with Fu since 2023, and I'm happy to maintain the game and servers.

0.9.x saw several new contributors as well, @andy5995 who did a ton of magic on the build system, and @Jammyjamjamman who made improvements to the Lua integration. @ph2708
helped with some artwork and testing, and has a group in Brazil that occasionally play the game.

## New Features & Improvements
* Core: Migrated to SDL2 by @winrid
* Gameplay: Added Map: Arena Fut by @andy5995 in https://github.com/netpanzer/netpanzer/pull/111
* Gameplay: Proximity Audio by @winrid in https://github.com/netpanzer/netpanzer/pull/188
* Gameplay: New bot modes by @futes
* Gameplay: New 'Desert' tileset/mapstyle by @Silvestre
* Gameplay: In the global destruct powerup (a red one), the unlucky player now loses all of his outposts too. by @futes
* Gameplay: New global unit powerups by @futes and artwork by @Silvestre.
* Gameplay: New game modes: game mode 3 (objective AND frag limit) - you must have both to win; game mode 4 (frag limit OR time limit). by @futes
* Gameplay: New respawn type: respawtype = 2 (random alternative) - It's random but never 2 consecutive respawns in the same spot. by @futes
* UI: New fancy styling for units, selectable on connect or by hitting "u". by @futes
* UI: New map styles by re-coloring our only tileset: SummerDay (the original one), IcyWinter, Moonlight, Scorched and Martian. by @futes
* UI: New "mute" option to mute a player in chat. by @futes
* UI: Support for rendering TTF fonts! This was a major refactor.
* UI: Blended dynamic-width font rendering & a fancy new font!
* UI: Wrapped Text Support With Variable Speed Scrolling by @winrid in https://github.com/netpanzer/netpanzer/pull/230
* UI: Improved switching to fullscreen on modern operating systems
* UI: Game Background and UI Updated by @winrid and @ph2708  in https://github.com/netpanzer/netpanzer/pull/98
* UI: Localization Support by @andy5995 in https://github.com/netpanzer/netpanzer/pull/155
* UI: Lua Scripting UI Support by @Jammyjamjamman in https://github.com/netpanzer/netpanzer/pull/200
* UI: Add "starting soon" message to winner screen by @andy5995 in https://github.com/netpanzer/netpanzer/pull/168
* UI: new extra top info bar with live ping stats to and from gameserver (toggle it with F7) by @futes
* UI: New "abandon" outpost options by @futes
* UI: fixed visualization of names in outposts (owners, outpost names, units in production etc). Now they are responsive based on string length by @futes 
* UI: SHIFT + m now toggles a bigger map. by @futes

## Bugs Fixed
* Gameplay: Mouse Movement Fixes (Jerky Movement etc) by @winrid in https://github.com/netpanzer/netpanzer/pull/121
* Gameplay: Fixing volume controls not affecting ambient sounds, explosions, etc. by @winrid in https://github.com/netpanzer/netpanzer/pull/266
* Gameplay: Fixed not being able to use arrow keys to navigate map if mouse is inside minimap.
* Gameplay: Bots now pick a random flag instead of a blank image.
* Gameplay: Pathfinding improvements to greatly reduce "node unreachable" messages from server. by @futes
* Gameplay: You might notice a slightly better reaction time for movements and shooting. by @futes
* UI: Potential crash when deleting/editing text in text input fields. by @andy5995 
* UI: Fixes for a crash during resolution selection when list is too large to fit on screen.
* Core: Fixed crashing if one master server is not available when hosting a game. by @winrid
* Core: Fix Game Config Not Saving by @winrid in https://github.com/netpanzer/netpanzer/pull/86
* Core: Fixes server using player.name instead of server.name config for dedicated servers by @winrid in https://github.com/netpanzer/netpanzer/pull/139
* Core: Fixes Random Crash by @winrid in https://github.com/netpanzer/netpanzer/pull/187

## Other Changes
* Gameplay: Deleted a timer in the input management - it could have been an advantage for those unfair player who use tools to bypass it... by @futes
* UI: Updated server list to hide old incompatible servers. It still shows *newer* incompatible servers to encourage upgrading.
* UI: added icons in ranking TAB that show if player is human or bot (must add a captcha soon..) - also the 'mute' icons have been replaced. by @futes 
* UI: Improved support for other languages than just English (more characters than just ASCII supported now).
* Core: Fixed c++17 compatibility (errors, some warnings). Build targets c++17 now.
* Core: Network layer improvements (fixed catching some exceptions by-value instead of by-ref, fixes some potential memory issues reported by valgrind). 
* Core: Added netpanzer.io to master server list.
* Core: Memory leak fixes by @futes
* Server: Anti Cheat Improvements - see README for new setting. The `game.anticheat` setting has been replaced.
* Server: Added ability to disable interactive console to run server as daemon.
* Server: Network scrambling is now default, if for some reason you want to deactivate it you can run the server in DEBUG mode (option -g) by @futes
* Server: New config: game.bots_allowed = false (or true)
* Server: New config: game.mapstyle = "[string]"
* Server: New config: game.gametype can now be also 3 (objective AND frag limit) or 4 (fraglimit OR timelimit)
* Build: Switching to the Meson build system. by @andy5995 
* Build: Documentation improvements. by @andy5995 @winrid 
* Build: Can now compile for MacOS by @andy5995 
* Build: Can now build and run server from Docker by @andy5995 
* Build: Introducing new "data dir" concept by @andy5995 
* Build: Able to compile on windows with MingW



-------------------------------------------------------------

The following are the changes from FU, in his original writing.


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




