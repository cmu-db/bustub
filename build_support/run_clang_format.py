#!/usr/bin/env python3

from pathlib import Path
from plumbum import local, FG

# Runs clang-format on everything in the specified directories.
fmt = local["clang-format-12"]
proj_root = Path.cwd().parent
dirs = ["src", "test"]

for _dir in dirs:
    _dir = proj_root / _dir
    files = list(_dir.glob("**/*.h")) + list(_dir.glob("**/*.cpp"))
    for file in files:
        fmt["-i", str(file), "--verbose", "--Werror"] & FG
