FROM andy5995/netpanzer-build-env:jammy
ARG DEBIAN_FRONTEND=noninteractive
RUN git clone --depth 1 https://github.com/netpanzer/netpanzer && \
        cd netpanzer && \
        meson setup _build && \
        cd _build && \
        ninja

FROM ubuntu:jammy-20231128
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
        libphysfs-dev \
        libsdl2-dev \
        libsdl2-mixer-dev \
        libsdl2-ttf-dev

RUN useradd -m npserver && passwd -d npserver
CMD ["/bin/bash","-l"]
