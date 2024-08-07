FROM ghcr.io/netpanzer/netpanzer:build-env-jammy-latest
ARG DEBIAN_FRONTEND=noninteractive
RUN apt update && apt -y upgrade

RUN mkdir /netpanzer
ADD meson* /netpanzer
ADD data /netpanzer/data
ADD docs /netpanzer/docs
ADD po /netpanzer/po
ADD src /netpanzer/src
ADD subprojects /netpanzer/subprojects
ADD tests /netpanzer/tests
ADD --chmod=700 setup-build.sh /netpanzer

RUN \
  cd /netpanzer && \
  ./setup-build.sh _build \
    -Dprefix=/usr \
    -Db_sanitize=none \
    -Dbuildtype=release -Dstrip=true && \
  cd _build && \
  ninja && \
  meson install --skip-subprojects

FROM ubuntu:jammy
LABEL org.opencontainers.image.source=https://github.com/netpanzer/netpanzer
COPY --from=0 /usr/bin/netpanzer /usr/bin
COPY --from=0 /usr/share/netpanzer /usr/share/netpanzer
COPY --from=0 /netpanzer/_build/po/* /usr/share/locale

ARG DEBIAN_FRONTEND=noninteractive
RUN apt update &&   \
    apt -y upgrade && \
    apt install -y  \
        liblua5.1-0 \
        libphysfs1 \
        libsdl2-2.0-0 \
        libsdl2-mixer-2.0-0 \
        libsdl2-ttf-2.0-0 \
        libasan6 \
        libubsan1 \
        sudo

RUN useradd -m npserver && passwd -d npserver && \
  echo "npserver ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

USER npserver
WORKDIR /home/npserver

CMD ["/bin/bash","-l"]
