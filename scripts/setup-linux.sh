#!/bin/bash
# setup-linux.sh - Rwal Linux Dependencies Installer

# Ensure execution halts immediately if an unhandled pipeline command fails
set -e

# --- Enforce Environment Boundaries ---
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
    echo "[ERROR] This script is target-scoped for Linux operating systems only."
    echo "Please execute './setup-windows.ps1' within an elevated PowerShell terminal."
    exit 1
fi

if [[ $EUID -ne 0 ]]; then
   echo "[ERROR] Administrative privileges missing. Please invoke this utility using 'sudo'."
   exit 1
fi

if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS_ID=$ID
else
    echo "[ERROR] Core system architecture file '/etc/os-release' missing. OS tracking impossible."
    exit 1
fi

# --- Accurate Desktop Environment Mapping ---
DE="Unknown"
if pgrep -x "gnome-shell" > /dev/null; then
    DE="GNOME"
elif pgrep -x "plasmashell" > /dev/null; then
    DE="KDE"
elif pgrep -x "Hyprland" > /dev/null; then
    DE="Hyprland"
elif [ -n "${XDG_CURRENT_DESKTOP}" ]; then
    DE="${XDG_CURRENT_DESKTOP}"
fi

echo "--- System Telemetry ---"
echo "Target OS: $OS_ID"
echo "Target DE: $DE"
echo "-----------------------"

# --- Deep Verification Routines ---
check_git()     { command -v git >/dev/null 2>&1; }
check_gxx()     { command -v g++ >/dev/null 2>&1; }
check_cmake()   { command -v cmake >/dev/null 2>&1; }
check_curl()    { command -v curl >/dev/null 2>&1 || pkg-config --exists libcurl 2>/dev/null; }
check_json()    { pkg-config --exists nlohmann_json 2>/dev/null || [ -f /usr/include/nlohmann/json.hpp ] || [ -f /usr/local/include/nlohmann/json.hpp ]; }
check_ncurses() { pkg-config --exists ncurses 2>/dev/null || [ -f /usr/include/ncurses.h ] || [ -f /usr/include/ncurses/ncurses.h ]; }
check_glib2()   { pkg-config --exists glib-2.0 2>/dev/null; }

# --- Map Functional Tools to System Names Explicitly ---
declare -A TOOLS_SATISFIED
TOOLS_SATISFIED[git]=1;     check_git     || TOOLS_SATISFIED[git]=0
TOOLS_SATISFIED[gxx]=1;     check_gxx     || TOOLS_SATISFIED[gxx]=0
TOOLS_SATISFIED[cmake]=1;   check_cmake   || TOOLS_SATISFIED[cmake]=0
TOOLS_SATISFIED[curl]=1;    check_curl    || TOOLS_SATISFIED[curl]=0
TOOLS_SATISFIED[json]=1;    check_json    || TOOLS_SATISFIED[json]=0
TOOLS_SATISFIED[ncurses]=1; check_ncurses || TOOLS_SATISFIED[ncurses]=0
TOOLS_SATISFIED[glib2]=1;   check_glib2   || TOOLS_SATISFIED[glib2]=0

# --- Distro-Specific Matrix Resolution ---
MISSING_PKGS=()
NEED_HYPRPAPER_PORTABLE=false

case "$OS_ID" in
    ubuntu|debian|linuxmint|pop)
        echo "[INFO] Syncing repository indexes..."
        apt-get update -qq
        INSTALL_CMD="apt-get install -y"

        [ ${TOOLS_SATISFIED[git]} -eq 0 ]     && MISSING_PKGS+=("git")
        [ ${TOOLS_SATISFIED[cmake]} -eq 0 ]   && MISSING_PKGS+=("cmake")
        [ ${TOOLS_SATISFIED[gxx]} -eq 0 ]     && MISSING_PKGS+=("g++")
        [ ${TOOLS_SATISFIED[curl]} -eq 0 ]    && MISSING_PKGS+=("libcurl4-openssl-dev")
        [ ${TOOLS_SATISFIED[json]} -eq 0 ]    && MISSING_PKGS+=("libnlohmann-json-dev")
        [ ${TOOLS_SATISFIED[ncurses]} -eq 0 ] && MISSING_PKGS+=("libncurses5-dev")
        [[ "$DE" == "GNOME" ]] && [ ${TOOLS_SATISFIED[glib2]} -eq 0 ] && MISSING_PKGS+=("libglib2.0-dev")
        
        # --- Handle Hyprland Wallpaper Requirements Safely ---
        if [[ "$DE" == "Hyprland" ]]; then
            if ! command -v hyprpaper >/dev/null 2>&1 && ! command -v swww >/dev/null 2>&1; then
                echo "[INFO] Neither hyprpaper nor swww found. Queueing hyprpaper installation..."
                if apt-cache show hyprpaper >/dev/null 2>&1; then
                    MISSING_PKGS+=("hyprpaper")
                elif apt-cache show swww >/dev/null 2>&1; then
                    MISSING_PKGS+=("swww")
                else
                    NEED_HYPRPAPER_PORTABLE=true
                fi
            else
                echo "[OK] Hyprland wallpaper daemon already present on system."
            fi
        fi
        ;;

    fedora|nobara)
        INSTALL_CMD="dnf install -y"

        [ ${TOOLS_SATISFIED[git]} -eq 0 ]     && MISSING_PKGS+=("git")
        [ ${TOOLS_SATISFIED[cmake]} -eq 0 ]   && MISSING_PKGS+=("cmake")
        [ ${TOOLS_SATISFIED[gxx]} -eq 0 ]     && MISSING_PKGS+=("gcc-c++")
        [ ${TOOLS_SATISFIED[curl]} -eq 0 ]    && MISSING_PKGS+=("libcurl-devel")
        [ ${TOOLS_SATISFIED[json]} -eq 0 ]    && MISSING_PKGS+=("nlohmann-json-devel")
        [ ${TOOLS_SATISFIED[ncurses]} -eq 0 ] && MISSING_PKGS+=("ncurses-devel")
        [[ "$DE" == "GNOME" ]] && [ ${TOOLS_SATISFIED[glib2]} -eq 0 ] && MISSING_PKGS+=("glib2-devel")
        
        if [[ "$DE" == "Hyprland" ]]; then
            if ! command -v hyprpaper >/dev/null 2>&1 && ! command -v swww >/dev/null 2>&1; then
                echo "[INFO] Neither hyprpaper nor swww found. Queueing hyprpaper installation..."
                if dnf list available hyprpaper >/dev/null 2>&1 || dnf list installed hyprpaper >/dev/null 2>&1; then
                    MISSING_PKGS+=("hyprpaper")
                elif dnf list available swww >/dev/null 2>&1 || dnf list installed swww >/dev/null 2>&1; then
                    MISSING_PKGS+=("swww")
                else
                    NEED_HYPRPAPER_PORTABLE=true
                fi
            else
                echo "[OK] Hyprland wallpaper daemon already present on system."
            fi
        fi
        ;;

    arch|manjaro|endeavouros)
        INSTALL_CMD="pacman -S --needed --noconfirm"

        [ ${TOOLS_SATISFIED[git]} -eq 0 ]     && MISSING_PKGS+=("git")
        [ ${TOOLS_SATISFIED[cmake]} -eq 0 ]   && MISSING_PKGS+=("cmake")
        [ ${TOOLS_SATISFIED[gxx]} -eq 0 ]     && MISSING_PKGS+=("gcc")
        [ ${TOOLS_SATISFIED[curl]} -eq 0 ]    && MISSING_PKGS+=("curl")
        [ ${TOOLS_SATISFIED[json]} -eq 0 ]    && MISSING_PKGS+=("nlohmann-json")
        [ ${TOOLS_SATISFIED[ncurses]} -eq 0 ] && MISSING_PKGS+=("ncurses")
        [[ "$DE" == "GNOME" ]] && [ ${TOOLS_SATISFIED[glib2]} -eq 0 ] && MISSING_PKGS+=("glib2")
        
        if [[ "$DE" == "Hyprland" ]]; then
            if ! command -v hyprpaper >/dev/null 2>&1 && ! command -v swww >/dev/null 2>&1; then
                MISSING_PKGS+=("hyprpaper") # Native on Arch repos
            fi
        fi
        ;;

    opensuse*)
        INSTALL_CMD="zypper install -y"

        [ ${TOOLS_SATISFIED[git]} -eq 0 ]     && MISSING_PKGS+=("git")
        [ ${TOOLS_SATISFIED[cmake]} -eq 0 ]   && MISSING_PKGS+=("cmake")
        [ ${TOOLS_SATISFIED[gxx]} -eq 0 ]     && MISSING_PKGS+=("gcc-c++")
        [ ${TOOLS_SATISFIED[curl]} -eq 0 ]    && MISSING_PKGS+=("libcurl-devel")
        [ ${TOOLS_SATISFIED[json]} -eq 0 ]    && MISSING_PKGS+=("nlohmann-json-devel")
        [ ${TOOLS_SATISFIED[ncurses]} -eq 0 ] && MISSING_PKGS+=("ncurses-devel")
        [[ "$DE" == "GNOME" ]] && [ ${TOOLS_SATISFIED[glib2]} -eq 0 ] && MISSING_PKGS+=("glib2-devel")
        
        if [[ "$DE" == "Hyprland" ]]; then
            if ! command -v hyprpaper >/dev/null 2>&1 && ! command -v swww >/dev/null 2>&1; then
                echo "[INFO] Neither hyprpaper nor swww found. Queueing hyprpaper installation..."
                if zypper se -x hyprpaper >/dev/null 2>&1; then
                    MISSING_PKGS+=("hyprpaper")
                elif zypper se -x swww >/dev/null 2>&1; then
                    MISSING_PKGS+=("swww")
                else
                    NEED_HYPRPAPER_PORTABLE=true
                fi
            else
                echo "[OK] Hyprland wallpaper daemon already present on system."
            fi
        fi
        ;;

    *)
        echo "[ERROR] Runtime OS targets are unknown or unsupported: $OS_ID"
        exit 1
        ;;
esac

# --- Targeted Operations Pipeline ---
if [ ${#MISSING_PKGS[@]} -gt 0 ]; then
    echo ""
    echo "[INFO] Queueing installation for targets: ${MISSING_PKGS[*]}"
    
    # Temporarily allow errors inside execution command block for granular error reports
    set +e
    $INSTALL_CMD "${MISSING_PKGS[@]}"
    INSTALL_STATUS=$?
    set -e

    if [ $INSTALL_STATUS -eq 0 ]; then
        echo ""
        echo "-----------------------"
        echo "STATUS: SUCCESS"
        echo "All core dependencies for $DE are fully set up."
    else
        echo ""
        echo "-----------------------"
        echo "STATUS: FAILED"
        echo "[ERROR] The installation routine exited unexpectedly. Verify connections or mirror flags."
        exit 1
    fi
else
    echo ""
    echo "-----------------------"
    echo "STATUS: SUCCESS"
    echo "All deep dependencies are already present on the host environment."
fi

# --- Fast Pre-Compiled Release Downloader Fallback ---
if [ "$NEED_HYPRPAPER_PORTABLE" = true ]; then
    echo "[INFO] hyprpaper is completely missing from system pools. Deploying official pre-compiled release..."
    
    # Detect latest hyprpaper release tag from GitHub API
    HYPRPAPER_VERSION="0.8.0"  # Fallback if API fails
    if command -v curl >/dev/null 2>&1; then
        LATEST_TAG=$(curl -sL "https://api.github.com/repos/hyprwm/hyprpaper/releases/latest" 2>/dev/null | grep -oP '"tag_name":\s*"\K[^"]+')
        if [ -n "$LATEST_TAG" ]; then
            HYPRPAPER_VERSION="${LATEST_TAG#v}"
        fi
    fi

    # Create safe isolated scratch directory
    TMP_DOWNLOAD_DIR=$(mktemp -d)
    
    # Download official pre-compiled x86_64 binary from Hyprland releases
    HYPRPAPER_URL="https://github.com/hyprwm/hyprpaper/releases/download/v${HYPRPAPER_VERSION}/hyprpaper-${HYPRPAPER_VERSION}-x86_64.tar.gz"
    echo "[INFO] Fetching hyprpaper v${HYPRPAPER_VERSION} from GitHub releases..."
    
    if command -v wget >/dev/null 2>&1; then
        wget -qO "$TMP_DOWNLOAD_DIR/hyprpaper.tar.gz" "$HYPRPAPER_URL"
    elif command -v curl >/dev/null 2>&1; then
        curl -sLo "$TMP_DOWNLOAD_DIR/hyprpaper.tar.gz" "$HYPRPAPER_URL"
    else
        echo "[ERROR] Neither wget nor curl is available for download."
        rm -rf "$TMP_DOWNLOAD_DIR"
        exit 1
    fi

    if [ -f "$TMP_DOWNLOAD_DIR/hyprpaper.tar.gz" ] && [ -s "$TMP_DOWNLOAD_DIR/hyprpaper.tar.gz" ]; then
        tar -xzf "$TMP_DOWNLOAD_DIR/hyprpaper.tar.gz" -C "$TMP_DOWNLOAD_DIR"
        # Copy binary to standard globally accessible system execution space
        if [ -f "$TMP_DOWNLOAD_DIR/hyprpaper" ]; then
            cp "$TMP_DOWNLOAD_DIR/hyprpaper" /usr/local/bin/
            chmod +x /usr/local/bin/hyprpaper
            echo "[SUCCESS] hyprpaper v${HYPRPAPER_VERSION} deployed to /usr/local/bin/hyprpaper"
        else
            echo "[ERROR] hyprpaper binary not found in extracted archive."
        fi
    else
        echo "[ERROR] Failed to download hyprpaper from GitHub releases."
        echo "  Manual install: https://github.com/hyprwm/hyprpaper/releases"
    fi
    
    rm -rf "$TMP_DOWNLOAD_DIR"
fi
