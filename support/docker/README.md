# Docker

## Run NetPanzer in a container

Copy docker-compose.yml and the opt directory to anywhere you like, such as
`$HOME/.local/netpanzer/docker`

Edit $HOME/.local/netpanzer/docker/opt/config/server.cfg` as desired. See
[SERVER-HOWTO.md](https://github.com/netpanzer/netpanzer/blob/master/docs/SERVER-HOWTO.md)
if you need help with the options.

To run NetPanzer in a docker container, in the directory containing
docker-compose.yml, use

    docker-compose up -d

To kill the server:

    docker-compose down

Also see the [docker compose
docs](https://docs.docker.com/compose/features-uses/) for more options and
ways to edit the compose yml in this directory.

You may alternatively wish to use from a specific release. You may edit the
compose file to use a differet tag. The images are at
https://github.com/netpanzer/netpanzer/pkgs/container/netpanzer
