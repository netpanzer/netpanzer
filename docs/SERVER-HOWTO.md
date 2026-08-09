# Dedicated Server and Configuration Files

## Docker

See
[support/docker](https://github.com/netpanzer/netpanzer/blob/master/support/docker)
for details.

## Linux

Here's a series of commands to setup a dedicated server that will survive server restarts and crashes.

First, we'll download the game server and start it, to test:

    cd
    mkdir netpanzer && cd netpanzer
    wget https://github.com/netpanzer/netpanzer/releases/download/v0.9.0/netpanzer-0.9.0-linux-x86.tar.gz
    tar -xvf netpanzer-0.9.0-linux-x86.tar.gz
    mkdir -p ~/.netpanzer/config
    echo "server.interactive_console = false" > ~/.netpanzer/config/server.cfg
    ./netpanzer -d

Now your server should be visible in the server browser. If not, figure out your networking issues.

Once that is done, hit ctrl+c to kill the server. Now we will make it a reliable server.

Create a new `netpanzer.service` file:

    sudo nano /etc/systemd/system/netpanzer.service

Put this in it:

    [Unit]
    Description=netpanzer server
    After=network.target
    StartLimitIntervalSec=1

    [Service]
    Type=simple
    Restart=always
    RestartSec=1
    User=winrid
    ExecStart=/home/yourusername/netpanzer/netpanzer -d -p 3031

    [Install]
    WantedBy=multi-user.target

Replace `yourusername` with your username.

Update systemd:

    sudo systemctl daemon-reload
    sudo systemctl enable netpanzer
    sudo systemctl start netpanzer

Now check the server's logs:

    sudo journalctl -u netpanzer -fn1000

Or to check the status:

    sudo systemctl status netpanzer

## Setting up Bots

Currently, to run bots you have to run more instances of the server. So create a new service like you did for `netpanzer` but call it
`np-bot-1` for example, and change

    ExecStart=/home/yourusername/netpanzer/netpanzer -d -p 3031

to:

    ExecStart=/home/yourusername/netpanzer/netpanzer -b localhost:3031 --game_config='/home/yourusername/.netpanzer/config/bot1.cfg'

and of course, change `yourusername` to your username.

## Server Configuration

In your home directory you will find a folder named `.netpanzer`.

Inside it, another folder named `config`, and inside that a file named `server.cfg`.
You might need to run the dedicated server once to generate this file.

This file will contain all the entries to configure your dedicated server.

Note: Not ALL the entries in server.cfg are related to the server, only those tagged as `server.` and `game.` - so let's see in detail the ones which are really important.

### Server Name

    server.name = "My Cool Server"

Your game server name which will be displayed in server browser etc.

### Port Number

    server.port = 3030

No reason to change this if you run a single server.

### Message of The Day

    server.motd = "Welcome to NetPanzer Server"

This is the message that appears to players when they connect to the game server.

### Detailed Logging

    server.logging = true

You can get more detailed logs by setting this to `true`.

### Public Server

    server.public = true

### Custom Master Servers

The master servers are servers that hold the server list.

So your server needs to ping it. Your game client needs to call it. We don't recommend changing this.

    server.masterservers = "netpanzer.io"

### Interactive Server Console

     server.interactive_console = true

You may want to set this too false to disable the interactive console, to run the server as a daemon.
Otherwise, the server will create excessive logs and 100% cpu/disk usage.

### Anti-Cheat

Anti-cheat is accomplished by kicking clients if they exceed a certain number of commands a second.

    server.command_burst_limit = 18

This sets the max consecutive number of "burst" packets from the client (packets within 125ms of each other).
This is to prevent people from using auto-clickers.

### Enable/Disable Bases

    game.enable_bases = true

By setting this to `false` you disable all bases in the map.

### Capture Mode

    game.base_capture_mode = 1

The kind of base capture, no capture, helipad capture, full base capture.

### Base Limit

    game.base_limit = 0

Max bases per player.

### Auto Kick

    game.autokicktime = 20

Kicks inactive player after given minutes.

### Multiple Players w/ Same IP

    game.allowmultiip = true

By setting this to 'false' you prevent people with same IP address to connect.

### Unit Profiles

    game.unit_profiles = "Manta, Panther1, Titan, Stinger, Bobcat, Bear, Archer, Wolf, Drake, Spanzer"

In this string separated by commas you can decide the available units in a game (the ones you see when you click on top of an outpost you own).

### Unit Spawn Counts

    game.unit_spawnlist = "1, 1, 1, 1, 1, 1, 1, 1, 1, 1"

Comma separated string which must reflect `game.unit_profiles`. Here you decide the starting number of units (on respawn).

### Admin Password

    game.adminpass = ""

Set a password to allow admin commands via chat in a game.
See the help section in your NetPanzer client for a full list of commands.

### Game Password

    game.gamepass = ""

You can set a password players must know to connect to your game server.

### Flag Change Timeout

    game.changeflagtime = 15

Timeout for flag change in minutes.

### Game Type

    game.gametype = 0

0=objective, 1=frag limit, 2=timelimit

### Max Players

    game.maxplayers = 8

### Max Units

    game.maxunits = 320

Max number of units (tanks) in the game - which also determines the max number of units per player: `maxunits/maxplayers`

### Time Limit

    game.timelimit = 30

Time limit in minutes.

### Frag Limit

    game.fraglimit = 300

### Enable/Disable Power Ups

    game.powerups = true

### Occupation Percentage

    game.occupationpercentage = 75

Decides outposts percentage to end an objective game.

### Enable/Disable Allies

    game.allowallies = true

### Low Score Limit

    game.lowscorelimit = -25

Players get kicked if their score is less than this value - allowed range is -15 to -100.

### Cloud Coverage

    game.cloudcoverage = 0

Clouds density.

### Wind Speed

    game.windspeed = 30

Wind affects bullet travel and cloud coverage movement.

### Respawn Type

    game.respawntype = 0

0=round robin, 1=random

### Starting Map

    game.map = "Two clans"

### Map List

    game.mapcycle = "Two clans"

Map cycle (comma separated list).

To use all maps:

    game.mapcycle = "Bad Neuburg,Bullet Hole,Cramped,Crop Circles,Duell,Heroes,Hill 221 (mod),Hill 221,Hurry up,Masters,Mittelerde,Open War,Operation Blue,Road to Abitos,Six Approaches,Tankdrive,The Pit,The Valley (mod),The Valley,Tight Quarters,Two clans (mod),Two clans,Two Villages,Walhalla,Wall-2,Wall,Wild Hun"

### From your PC - Checking (setting) up your Network

To run a NetPanzer game server visible to everybody on the internet you must be able to forward port 3030 to your local PC address.

Generally this can be done in your modem/router control panel (look for the 'NAT' or 'Port Forwarding' section etc). You can google for your router brand manual and follow one of the many tutorials about port forwarding (http://en.wikipedia.org/wiki/Port_forwarding - You might already have done such a thing when configuring Emule or other file sharing tools!). You can obtain more information about your network using command 'ipconfig' in cmd. (start -> run -> type 'cmd' -> type 'ipconfig' ) 'IPv4 address' is your network device address (your machine local address) - 'Default gateway' is likely to be your modem/router address and typing that address in a browser might send you to its control panel.

Also check that Windows Firewall - and/or other antivirus software which may act as firewall - are not interfering.
