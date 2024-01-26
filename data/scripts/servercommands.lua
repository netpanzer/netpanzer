local splitsay = function(player, prefix, tbl)
    local s = "";
    local i = 0;
    while ( i < #tbl )
    do
        i = i + 1;
        if ( #s + #tbl[i] > 80 ) then
            netpanzer.serversayto(player, prefix .. s);
            s = tbl[i];
        else
            if ( #s > 0 ) then s = s .. ',' end
            s = s .. tbl[i];
        end
    end

    if ( #s > 0 ) then
        netpanzer.serversayto(player, prefix .. s);
    end
end;

local boolToString = function(b)
    return b and 'true' or 'false';
end;

local base_modes = {};
base_modes[0] = "Can't capture";
base_modes[1] = "Helipad capture";
base_modes[2] = "Full base capture";

ServerCommands =
{
    say_help = "Says something to all players as server.",
    say = function(param, player)
        if param then
            netpanzer.serversay(param);
        end
    end,

    listplayers_help = "List the players with their Id",
    listplayers = function(param, player)
        netpanzer.serversayto(player, 'Listing players:');
        local players = netpanzer.listplayers();
        local i = 0;
        while ( i < #players )
        do
            i = i + 1;
            local p = players[i];
            netpanzer.serversayto(player, p.id .. " - " .. p.name .. " ip: " .. p.ip .. " s.ping: " .. p.s_ping);
        end
    end,

    kick_help = "Kicks a player, use the player number (starts in 0)",
    kick = function(param, player)
        if param then
            netpanzer.kick(param);
        end
    end,

    map_help = "Change the map",
    map = function(param, player)
        netpanzer.serversayto(player, netpanzer.map(param));
    end,

    listmaps_help = "List the available maps in server",
    listmaps = function(param, player)
        netpanzer.serversayto(player, 'Listing maps:');
        splitsay(player, '', netpanzer.listmaps());
    end,

    listprofiles_help = "List the available unit profiles in server",
    listprofiles = function(param, player)
        netpanzer.serversayto(player, 'Listing profiles:');
        splitsay(player, '', netpanzer.listprofiles());
    end,

    listcommands_help = "List the server commands",
    listcommands = function(param, player)
        netpanzer.serversayto(player, 'Listing commands:');
        local out = {};
        for k,v in pairs(ServerCommands) do
            if type(v) == "function" then
                table.insert(out,k);
            end
        end
        splitsay(player, '', out);
    end,

    _help = "Type /server help <wanted_command> or /server listcommands",
    help_help = "Provides this kind of help",
    help = function(param, player)
        local ht = ServerCommands[param .. "_help"];
        if ht then
            netpanzer.serversayto( player, param .. ": " .. ht);
        else
            netpanzer.serversayto( player, "Help not found for " .. param .. ". Use /server listcommands");
        end
    end,

    autokick_help = "sets the idle time for kicking a player automatically",
    autokick = function(param, player)
        if ( param == "" ) then
            netpanzer.serversayto( player, "autokick time is " .. config.game.autokicktime);
        else
            config.game.autokicktime = param;
            netpanzer.serversayto( player, "autokick time was set to " .. config.game.autokicktime);
        end
    end,

    bases_help = "Shows if the bases are enabled on map loading",
    bases = function(param, player)
        netpanzer.serversayto( player, "#Bases are " .. ((config.game.enable_bases == true) and 'enabled' or 'disabled'));
    end,

    enablebases_help = "Enables the bases on next map loading",
    enablebases = function(param, player)
        netpanzer.serversayto( player, (config.game.enable_bases == true)
                                       and "#Bases are already enabled"
                                       or  "#Bases enabled and will be loaded at next map change");

        config.game.enable_bases = true;
    end,

    disablebases_help = "Disables the bases on next map loading",
    disablebases = function(param, player)
        netpanzer.serversayto( player, (config.game.enable_bases == false)
                                       and "#Bases are already disabled"
                                       or  "#Bases disabled and won't be loaded at next map change");

        config.game.enable_bases = false;
    end,

    multiip_help = "Shows if is allowed to connect multiple times from same IP address",
    multiip = function(param, player)
        netpanzer.serversayto( player, "#Multiple connections from same IP are " .. ((config.game.allowmultiip == true) and 'allowed' or 'forbidden'));
    end,

    allowmultiip_help = "Allows multiple connections from the same IP address",
    allowmultiip = function(param, player)
        netpanzer.serversayto( player, "Allowing multiple connections from the same IP address");
        config.game.allowmultiip = true;
    end,

    forbidemultiip_help = "Forbides multiple connections from the same IP address",
    forbidemultiip = function(param, player)
        netpanzer.serversayto( player, "Forbidding multiple connections from the same IP address");
        config.game.allowmultiip = false;
    end,

    unitprofiles_help = "sets or get list of unit profiles loaded",
    unitprofiles = function(param, player)
        if ( param == "" ) then
            netpanzer.serversayto( player, "Unit profile list: " .. config.game.unit_profiles);
        else
            config.game.unit_profiles = param;
            netpanzer.serversayto( player, "Unit profile list set to: " .. config.game.unit_profiles);
        end
    end,

    unitspawnlist_help = "sets or get the unitspawnlist",
    unitspawnlist = function(param, player)
        if ( param == "" ) then
            netpanzer.serversayto( player, "Unit spawn list: " .. config.game.unit_spawnlist);
        else
            config.game.unit_spawnlist = param;
            netpanzer.serversayto( player, "Unit spawn list set to: " .. config.game.unit_spawnlist);
        end
    end,

    baselimit_help = "sets or get the base limit for players",
    baselimit = function(param, player)
        if ( param == "" ) then
            netpanzer.serversayto( player, "Base limit: " .. config.game.base_limit);
        else
            config.game.base_limit = param;
            netpanzer.serversayto( player, "Base limit set to: " .. config.game.base_limit);
        end
    end,

    basemode_help = "sets or get the base capture mode: 0- no capture, 1- helipad capture, 2- all base capture",
    basemode = function(param, player)
        if ( param == "" ) then
            netpanzer.serversayto( player, "Base capture mode: " .. config.game.base_capture_mode .. "- " .. base_modes[config.game.base_capture_mode]);
        else
            local v = tonumber(param);
            if ( v ~= nil and v >= 0 and v <= 2 ) then
                config.game.base_capture_mode = v;
                netpanzer.serversayto( player, "Base capture mode set to: " .. config.game.base_capture_mode .. "- " .. base_modes[config.game.base_capture_mode]);
            else
                netpanzer.serversayto( player, "Capture mode is invalid");
            end
        end
    end,

    gamepass_help = "sets or get the password for connecting to the server",
    gamepass = function(param, player)
        if ( param == "" ) then
            if ( config.game.gamepass == "" ) then
                netpanzer.serversayto( player, "There is no password set");
            else
                netpanzer.serversayto( player, "Current game password: " .. config.game.gamepass);
            end
        else
            if ( param == "." ) then
                config.game.gamepass = "";
                netpanzer.serversayto( player, "Game password was removed");
            else
                config.game.gamepass = param;
                netpanzer.serversayto( player, "Game password set to: " .. config.game.gamepass);
            end

        end
    end,

    countdown_help = "Do a countdown, use 'countdown <time> <message>'",
    countdown = function(param, player)
        if game.hasTask('countdown') then
            netpanzer.serversayto( player, "There is already a countdown running");
            return;
        end
        local counttime, message = string.match(param, "(%d+) *(.*)");
        counttime = tonumber(counttime) or 5;

        if counttime > 20 then
            netpanzer.serversayto( player, "Time is too big");
            return;
        end

        message = (message and message:find("%S") and message) or "Countdown...";
        local count = 0;
        netpanzer.serversay(message .. " " .. counttime);

        game.addTask('countdown', function()
                count = count + game.frametime;
                if count > 1.0 then
                    count = count - 1.0;
                    counttime = counttime - 1;
                    if counttime == 0 then
                        netpanzer.serversay(message .. " FIGHT!!!!");
                    else
                        netpanzer.serversay(message .. " " .. counttime);
                    end
                end
                return counttime == 0;
            end);
    end,

    flagtimer_help = "sets the time for changing flag",
    flagtimer = function(param, player)
        if ( param == "" ) then
            netpanzer.serversayto( player, "flagtimer time is " .. config.game.changeflagtime);
        else
            config.game.changeflagtime = param;
            netpanzer.syncflagtimer();
            netpanzer.serversayto( player, "flagtimer time was set to " .. config.game.changeflagtime);
        end
    end,

    destroyunits_help = "destroy some player units",
    destroyunits = function(param, player)
        if ( param == "" ) then
            netpanzer.serversayto( player, "You need to specify the player number to destroy units");
        else
            netpanzer.destroyplayerunits(param);
            netpanzer.serversayto( player, "The units should be history");
        end
    end


};
