FROM andy5995/netpanzer-build-env:jammy

RUN git clone --depth 1 https://github.com/netpanzer/netpanzer && \
        cd netpanzer && \
        ./setup-build.sh _build -Db_sanitize=none && \
        cd _build && \
        ninja

FROM ubuntu:jammy
COPY --from=0 /netpanzer/_build/netpanzer /netpanzer/
COPY --from=0 /netpanzer/cache /netpanzer/cache
COPY --from=0 /netpanzer/fonts /netpanzer/fonts
COPY --from=0 /netpanzer/maps /netpanzer/maps
COPY --from=0 /netpanzer/pics /netpanzer/pics
COPY --from=0 /netpanzer/powerups /netpanzer/powerups
COPY --from=0 /netpanzer/scripts /netpanzer/scripts
COPY --from=0 /netpanzer/sound /netpanzer/sound
COPY --from=0 /netpanzer/units /netpanzer/units
COPY --from=0 /netpanzer/wads /netpanzer/wads

ARG DEBIAN_FRONTEND=noninteractive
RUN apt update &&   \
    apt -y upgrade && \
    apt install -y  \
        lua5.1-0 \
        libphysfs1 \
        libsdl2-2.0-0 \
        libsdl2-mixer-2.0-0 \
        libsdl2-ttf-2.0-0

RUN useradd -m npserver && passwd -d npserver

CMD ["/bin/bash","-l"]
