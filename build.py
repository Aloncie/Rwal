#!/usr/bin/env python3
"""
build.py — Cross-platform build wrapper for Rwal
Usage:
  python build.py [--clean] [--mode tui|cli|all] [--preset <name>] [--test]

  --help or -h — Show this help
  --clean — Remove the build directory and rebuild
  --test  — Build with testing enabled (sets -DBUILD_TESTING=ON)

Modes (Linux only):
  all   — TUI + CLI combined (default)
  tui   — TUI only
  cli   — CLI only
"""

import subprocess
import sys
import os
import shutil
import platform


def run(cmd, exit_on_fail=True):
    print(f"\033[36m[BUILD]\033[0m {cmd}")
    result = subprocess.run(cmd, shell=True)
    if exit_on_fail and result.returncode != 0:
        sys.exit(result.returncode)
    return result.returncode


def reload_from_registry(var_name):
    result = subprocess.run(
        ['powershell', '-NoProfile', '-Command',
         f'[Environment]::GetEnvironmentVariable("{var_name}","User")'],
        capture_output=True, text=True
    )
    return result.stdout.strip()


def main():
    is_windows = platform.system() == "Windows"

    # Separate boolean flags
    clean_flag = False
    test_flag = False

    # Collect remaining args for mode/preset processing
    args = sys.argv[1:]

    # Process flags
    for a in sys.argv[1:]:
        if a in ("--help", "-h"):
            print(__doc__)
            sys.exit(0)
        elif a == "--clean":
            clean_flag = True
        elif a == "--test":
            test_flag = True

    # Remove flags from args so we can parse positional options
    args = [a for a in args if a not in ("--clean", "--test")]

    if clean_flag:
        build_dir = "build"
        if os.path.exists(build_dir):
            print(f"\033[33m[CLEAN]\033[0m Removing {build_dir}/")
            shutil.rmtree(build_dir)

    # Extra CMake arguments
    cmake_extra = ""
    if test_flag:
        cmake_extra += " -DBUILD_TESTING=ON"

    # --- Windows path ---
    if is_windows:
        preset = "windows-release"
        if "--preset" in args:
            idx = args.index("--preset")
            if idx + 1 < len(args):
                preset = args[idx + 1]

        print("\033[35m=== Rwal Build — Windows ===\033[0m")

        vcpkg_root = reload_from_registry("VCPKG_ROOT")
        if not vcpkg_root:
            print("\033[31m[ERROR]\033[0m VCPKG_ROOT not set. Please run: python setup.py")
            sys.exit(1)
        os.environ["VCPKG_ROOT"] = vcpkg_root
        print(f"\033[36m[INFO]\033[0m VCPKG_ROOT = {vcpkg_root}")

        run(f"cmake --preset {preset} {cmake_extra}")
        run(f"cmake --build build/{preset} --preset {preset}")
        print("\033[32m[SUCCESS]\033[0m Build complete.")
        return

    # --- Linux path ---
    mode = "all"
    if "--mode" in args:
        idx = args.index("--mode")
        if idx + 1 < len(args):
            mode = args[idx + 1]

    preset_map = {
        "all": "linux-release",
        "tui": "linux-release-tui",
        "cli": "linux-release-cli",
    }
    preset = preset_map.get(mode, "linux-release")

    print(f"\033[35m=== Rwal Build — Linux ({mode.upper()}) ===\033[0m")

    run(f"cmake --preset {preset} {cmake_extra}")
    run(f"cmake --build build/{preset}")
    print("\033[32m[SUCCESS]\033[0m Build complete.")


if __name__ == "__main__":
    main()
