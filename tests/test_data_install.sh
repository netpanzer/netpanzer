#!/bin/sh
# Test for installed data

# As of now, this can be run manually or in the CI, not by 'meson test'.
# The data must be installed to get an accurate test. See support/Notes.md for
# details about installing data for testing.

set -ev

SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy HOME=$PWD ./netpanzer &
./netpanzer &
sleep 10s

if pgrep netpanzer > /dev/null; then
  pkill netpanzer
  exit $?
else
  exit 1
fi

