#!/usr/bin/env python3
"""
build.py — Cross-platform build wrapper for Rwal
Usage: python build.py [--clean] [--preset <name>]
"""

import subprocess
import sys
import os
import shutil
import platform


def run(cmd, exit_on_fail=True):
    """Run a command, print it. If exit_on_fail, stop on non-zero exit."""
    print(f"\033[36m[BUILD]\033[0m {cmd}")
    result = subprocess.run(cmd, shell=True)
    if exit_on_fail and result.returncode != 0:
        sys.exit(result.returncode)
    return result.returncode


def reload_from_registry(var_name):
    """Read a User-scope environment variable from the Windows registry."""
    result = subprocess.run(
        ['powershell', '-NoProfile', '-Command',
         f'[Environment]::GetEnvironmentVariable("{var_name}","User")'],
        capture_output=True, text=True
    )
    return result.stdout.strip()


def main():
    is_windows = platform.system() == "Windows"

    # Handle --clean
    if "--clean" in sys.argv:
        build_dir = "build"
        if os.path.exists(build_dir):
            print(f"\033[33m[CLEAN]\033[0m Removing {build_dir}/")
            shutil.rmtree(build_dir)
        else:
            print(f"\033[33m[CLEAN]\033[0m {build_dir}/ not found, nothing to clean")
        sys.exit(0)

    # --- Windows path ---
    if is_windows:
        preset = "windows-release"
        if "--preset" in sys.argv:
            idx = sys.argv.index("--preset")
            if idx + 1 < len(sys.argv):
                preset = sys.argv[idx + 1]

        print("\033[35m=== Rwal Build — Windows ===\033[0m")

        # 1. Reload VCPKG_ROOT and QTDIR from the registry (User scope)
        vcpkg_root = reload_from_registry("VCPKG_ROOT")
        qt_dir = reload_from_registry("QTDIR")

        # 2. If they are missing, the user must run setup first
        if not vcpkg_root:
            print("\033[31m[ERROR]\033[0m VCPKG_ROOT not set. Please run: python setup.py")
            sys.exit(1)
        if not qt_dir:
            print("\033[31m[ERROR]\033[0m QTDIR not set. Please run: python setup.py")
            sys.exit(1)

        # Push into the current session
        os.environ["VCPKG_ROOT"] = vcpkg_root
        os.environ["QTDIR"] = qt_dir

        print(f"\033[36m[INFO]\033[0m VCPKG_ROOT = {vcpkg_root}")
        print(f"\033[36m[INFO]\033[0m QTDIR = {qt_dir}")

        # 3. Build
        run(f"cmake --preset {preset}")
        run(f"cmake --build build/{preset} --preset {preset}")
        print("\033[32m[SUCCESS]\033[0m Build complete.")
        return

    # --- Linux path ---
    print("\033[35m=== Rwal Build — Linux ===\033[0m")
    preset = "linux-release"
    if "--preset" in sys.argv:
        idx = sys.argv.index("--preset")
        if idx + 1 < len(sys.argv):
            preset = sys.argv[idx + 1]

    qt_dir = os.environ.get("QTDIR", "")
    if not qt_dir:
        print("\033[33m[WARN]\033[0m QTDIR not set. CMake will search default paths.")

    run(f"cmake --preset {preset}")
    run(f"cmake --build build/{preset}")
    print("\033[32m[SUCCESS]\033[0m Build complete.")


if __name__ == "__main__":
    main()
