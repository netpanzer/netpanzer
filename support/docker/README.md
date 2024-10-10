# Docker

Use `docker pull ghcr.io/netpanzer/netpanzer:latest' to get the latest image.
You can view the architectures available at
https://github.com/netpanzer/netpanzer/pkgs/container/netpanzer and clicking
the "OS/Arch" tab.

## Run a dedicated server in a container

Copy docker-compose.yml and the opt directory to anywhere you like, such as
`$HOME/.local/netpanzer`

Edit $HOME/.local/netpanzer/opt/config/server.cfg` as desired. See
[SERVER-HOWTO.md](https://github.com/netpanzer/netpanzer/blob/master/docs/SERVER-HOWTO.md)
if you need help with the options.

To run NetPanzer in a docker container, in the directory containing
docker-compose.yml, use

    docker-compose up -d

To kill the server:

    docker-compose down

If you wish to run a bot in the container after it's started:

    docker exec --user npserver -it netpanzer-server \
        netpanzer -b localhost:3031 --game_config=.netpanzer/config/bot-khan.cfg

Also see the [docker compose
docs](https://docs.docker.com/compose/features-uses/) for more options and
ways to edit the compose yml in this directory.

You may alternatively wish to use from a specific release. You may edit the
compose file to use a different tag.
