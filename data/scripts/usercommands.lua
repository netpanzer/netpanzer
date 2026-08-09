UserCommands =
{
    say_help = "Says something to all players",
    say = function(param)



    if param == "aff" then

            netpanzer.say("   ###    ######## ########"); 
            netpanzer.say("  ## ##   ##       ##      ");
            netpanzer.say(" ##   ##  ##       ##      ");
            netpanzer.say("##     ## ######   ######  ");
            netpanzer.say("######### ##       ##      ");
            netpanzer.say("##     ## ##       ##      ");
            netpanzer.say("##     ## ##       ##      ");


    elseif param == "war" then

            netpanzer.say("##      ##    ###    ########     #### "); 
            netpanzer.say("##  ##  ##   ## ##   ##     ##    #### ");
            netpanzer.say("##  ##  ##  ##   ##  ##     ##    #### ");
            netpanzer.say("##  ##  ## ##     ## ########      ##  ");
            netpanzer.say("##  ##  ## ######### ##   ##           ");
            netpanzer.say("##  ##  ## ##     ## ##    ##     #### ");
            netpanzer.say(" ###  ###  ##     ## ##     ##    #### ");  


    elseif param == "help" then

            netpanzer.say("##    ## ######## ##       #######  "); 
            netpanzer.say("##    ## ##       ##       ##    ## ");
            netpanzer.say("##    ## ##       ##       ##    ## ");
            netpanzer.say("######## ######   ##       #######  ");
            netpanzer.say("##    ## ##       ##       ##       ");
            netpanzer.say("##    ## ##       ##       ##       ");
            netpanzer.say("##    ## ######## ######## ##       ");  


    elseif param == "powned" then

			netpanzer.say("#####   ####  #    # #    # ###### #####");
			netpanzer.say("#    # #    # #    # ##   # #      #    #");
			netpanzer.say("#    # #    # #    # # #  # #####  #    #");
			netpanzer.say("#####  #    # # ## # #  # # #      #    #");
			netpanzer.say("#      #    # ##  ## #   ## #      #    #");
			netpanzer.say("#       ####  #    # #    # ###### #####");


    elseif param == "omg" then

			netpanzer.say(" ####  #    #  ####");
			netpanzer.say("#    # ##  ## #    #");
			netpanzer.say("#    # # ## # #");
			netpanzer.say("#    # #    # #  ###");
			netpanzer.say("#    # #    # #    #");
			netpanzer.say(" ####  #    #  ####");


    elseif param == "looser" then

                        netpanzer.say("#                                             ###");
                        netpanzer.say("#        ####   ####   ####  ###### #####     ###");
                        netpanzer.say("#       #    # #    # #      #      #    #    ###");
                        netpanzer.say("#       #    # #    #  ####  #####  #    #     #");
                        netpanzer.say("#       #    # #    #      # #      #####");
                        netpanzer.say("#       #    # #    # #    # #      #   #     ###");
                        netpanzer.say("#######  ####   ####   ####  ###### #    #    ###");
 

    elseif param == "holy" then

                        netpanzer.say("#     #                         #####                    ###");
                        netpanzer.say("#     #  ####  #      #   #    #     # #    # # #####    ###");
                        netpanzer.say("#     # #    # #       # #     #       #    # #   #      ###");
                        netpanzer.say("####### #    # #        #       #####  ###### #   #       #");
                        netpanzer.say("#     # #    # #        #            # #    # #   #");
                        netpanzer.say("#     # #    # #        #      #     # #    # #   #      ###");
                        netpanzer.say("#     #  ####  ######   #       #####  #    # #   #      ###");



    else
      netpanzer.say(param)
      end
    end,




    teamsay_help = "Says something to team players",
    teamsay = function(param)
        if param then
            netpanzer.teamsay(param);
        end
    end,

    server_help = "Sends the command to the server";
    server = function(param)
        if param then
            netpanzer.say('/' .. param);
        end
    end,

    listcommands_help = "List all the commands";
    listcommands = function(param)
        local out;
        for k,v in pairs(UserCommands) do
            if type(v) == "function" then
                if out then
                    out = out .. ", " .. k;
                else
                    out = k;
                end
            end
        end
        netpanzer.scriptmessage(out);
    end,

    _help = "Type /help <wanted_command> or /listcommands",
    help_help = "Provides this kind of help",
    help = function(param)
        local ht = UserCommands[param .. "_help"];
        if ht then
            netpanzer.scriptmessage(param .. ": " .. ht);
        else
            netpanzer.scriptmessage("Help not found for " .. param .. ". Use /listcommands");
        end
    end,

    quit_help = "Quits NetPanzer.",
    quit = function(param)
       netpanzer.quit();
    end,

    -- doesn't work as expected, needs checking.
    --disconnect_help = "Disconnects from current server.",
    --disconnect = function(param)
    --    GameManager:quitNetPanzerGame();
    --end,

    test = function(param)
        netpanzer.scriptmessage("the thing is: " .. config.game.unit_profiles);
    end,
    
    countdown_help = "Do a countdown.",
    countdown = function(param)
        if game.hasTask('countdown') then
            netpanzer.scriptmessage("There is already a countdown running");
            return;
        end
        local counttime, message = string.match(param, "(%d+) *(.*)");

        counttime = tonumber(counttime) or 5;
        if counttime > 20 then
            netpanzer.scriptmessage("Time is too big");
            return;
        end

        message = (message and message:find("%S") and message) or "Countdown...";
        local count = 0;
        netpanzer.say(message .. " " .. counttime);

        game.addTask('countdown', function()
            count = count + game.frametime;
            if count > 1.0 then
                count = count - 1.0;
                counttime = counttime - 1;
                if counttime == 0 then
                    netpanzer.say(message .. " FIGHT!!!!");
                else
                    netpanzer.say(message .. " " .. counttime);
                end
            end
            return counttime == 0;
        end);
    end
};

