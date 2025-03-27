#!/usr/bin/env python3

import os
import re
import sys

# Set the path to the meson.build file
here = os.path.dirname(os.path.realpath(__file__))
meson_build_path = os.path.join(here, '..', '..', 'meson.build')

# Try to read and extract the version from the meson.build file
try:
    with open(meson_build_path, 'r') as f:
        version_found = False
        for line in f:
            match = re.search(r"version:\s*'([^']+)'", line)
            if match:
                print(match.group(1))
                version_found = True
                break
        if not version_found:
            raise ValueError("Version not found in meson.build")
except Exception:
    # If there's an error (e.g., file not found or version not found), exit with a non-zero error code
    sys.exit(1)
