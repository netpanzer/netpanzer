#!/bin/sh

# Be verbose, exit on error
set -ev

sudo apt update
sudo apt upgrade -y
sudo apt install -y \
  gettext \
  liblua5.1-0-dev \
  libphysfs-dev \
  libsdl2-dev \
  libsdl2-mixer-dev \
  libsdl2-ttf-dev
