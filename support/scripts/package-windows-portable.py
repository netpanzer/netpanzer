#!/usr/bin/env python3
"""Assemble the Windows portable zip from a MinGW meson build.

Copies netpanzer.exe, every non-system DLL it needs (resolved recursively
from the build tree and the MinGW toolchain), the game data and the docs
into one folder and zips it. Fails if a DLL import cannot be found and is
not a Windows system DLL, so a zip that cannot start is never produced.

Usage: package-windows-portable.py --build-dir _build --out-dir dist
"""

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

SOURCE_ROOT = Path(__file__).resolve().parents[2]
DOC_FILES = ["README.md", "COPYING.txt", "CHANGELOG.md", "docs/SERVER-HOWTO.md"]
DLL_NAME = re.compile(r"DLL Name:\s*(\S+)", re.IGNORECASE)


def find_objdump():
    for name in ("objdump", "x86_64-w64-mingw32-objdump"):
        path = shutil.which(name)
        if path:
            return path
    sys.exit("objdump not found on PATH")


def dll_imports(objdump, binary):
    out = subprocess.run([objdump, "-p", str(binary)], check=True,
                         capture_output=True, text=True).stdout
    return DLL_NAME.findall(out)


def project_version(build_dir):
    out = subprocess.run(["meson", "introspect", "--projectinfo", str(build_dir)],
                         check=True, capture_output=True, text=True).stdout
    return json.loads(out)["version"]


def index_dlls(dirs):
    """Map lower-case DLL name -> path, first directory wins."""
    found = {}
    for d in dirs:
        if not d.is_dir():
            continue
        for path in d.rglob("*.dll"):
            found.setdefault(path.name.lower(), path)
    return found


def toolchain_dll_dirs():
    """Directories holding the MinGW runtime DLLs (libgcc, libstdc++, ...).

    gcc on PATH may be a shim, so ask the compiler where libgcc lives:
    <root>/lib/gcc/<triplet>/<version>/libgcc.a, and the DLLs are in
    <root>/bin and <root>/<triplet>/bin.
    """
    for name in ("gcc", "x86_64-w64-mingw32-gcc"):
        if not shutil.which(name):
            continue
        libgcc = subprocess.run([name, "-print-libgcc-file-name"], check=True,
                                capture_output=True, text=True).stdout.strip()
        root = Path(libgcc).resolve().parents[4]
        triplet = Path(libgcc).resolve().parents[1].name
        return [root / "bin", root / triplet / "bin"]
    return []


def system_dll_dirs():
    root = os.environ.get("SystemRoot", r"C:\Windows")
    return [Path(root) / "System32"]


def is_system_dll(name):
    if name.lower().startswith(("api-ms-win-", "ext-ms-")):
        return True
    return any((d / name).exists() for d in system_dll_dirs())


def collect_dlls(objdump, exe, search):
    needed = {}
    missing = set()
    queue = [exe]
    seen = set()
    while queue:
        binary = queue.pop()
        for name in dll_imports(objdump, binary):
            key = name.lower()
            if key in seen:
                continue
            seen.add(key)
            if key in search:
                needed[key] = search[key]
                queue.append(search[key])
            elif not is_system_dll(name):
                missing.add(name)
    return needed, missing


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--build-dir", default="_build", type=Path)
    parser.add_argument("--out-dir", default="dist", type=Path)
    parser.add_argument("--version", help="defaults to the meson project version")
    args = parser.parse_args()

    build_dir = args.build_dir.resolve()
    exe = build_dir / "netpanzer.exe"
    if not exe.is_file():
        sys.exit(f"{exe} not found, build first")

    version = args.version or project_version(build_dir)
    objdump = find_objdump()

    search = index_dlls([build_dir] + toolchain_dll_dirs())

    dlls, missing = collect_dlls(objdump, exe, search)
    if missing:
        sys.exit("Unresolved DLL imports: " + ", ".join(sorted(missing)))

    name = f"netpanzer-{version}-windows-portable"
    stage = args.out_dir.resolve() / name
    if stage.exists():
        shutil.rmtree(stage)
    stage.mkdir(parents=True)

    shutil.copy2(exe, stage / "netpanzer.exe")
    for path in sorted(dlls.values()):
        shutil.copy2(path, stage / path.name)
        print(f"bundled {path.name} from {path.parent}")

    shutil.copytree(SOURCE_ROOT / "data", stage / "data")
    for doc in DOC_FILES:
        dest = stage / doc
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(SOURCE_ROOT / doc, dest)

    archive = shutil.make_archive(str(stage), "zip", root_dir=stage.parent,
                                  base_dir=name)
    print(f"wrote {archive}")


if __name__ == "__main__":
    main()
