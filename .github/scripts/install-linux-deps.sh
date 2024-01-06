#!/bin/sh

# Be verbose, exit on error
set -ev

sudo apt update
sudo apt upgrade -y
sudo apt install -y \
  libsdl2-dev \
  libsdl2-mixer-dev \
  libsdl2-ttf-dev \
  python3 \
  python3-setuptools

# Installation of meson and ninja will be added here after the scons
# build has been deprecated
