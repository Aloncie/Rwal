#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "[ERROR] Please run this script with sudo: sudo $0"
   exit 1
fi

if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS_NAME=$NAME
    OS_ID=$ID
else
    echo "[ERROR] Cannot detect Operating System."
    exit 1
fi

DE="Unknown"
if [[ "$XDG_CURRENT_DESKTOP" == *"GNOME"* ]]; then
    DE="GNOME"
elif [[ "$XDG_CURRENT_DESKTOP" == *"KDE"* ]]; then
    DE="KDE"
fi

echo "--- System Info ---"
echo "OS: $OS_NAME ($OS_ID)"
echo "DE: $DE"
echo "-------------------"

COMMON_PKGS=""
EXTRA_PKGS=""

case "$OS_ID" in
    ubuntu|debian|linuxmint|pop)
        COMMON_PKGS="git cmake g++ qtbase5-dev libncurses5-dev libcurl4-openssl-dev libnlohmann-json-dev"
        [[ "$DE" == "GNOME" ]] && EXTRA_PKGS="libglib2.0-dev"
        echo "[INFO] Installing for $OS_ID..."
        apt update && apt install -y $COMMON_PKGS $EXTRA_PKGS
        ;;
    fedora|nobara)
        COMMON_PKGS="git cmake gcc-c++ qt5-qtbase-devel ncurses-devel libcurl-devel nlohmann-json-devel"
        [[ "$DE" == "GNOME" ]] && EXTRA_PKGS="glib2-devel"
        echo "[INFO] Installing for $OS_ID..."
        dnf install -y $COMMON_PKGS $EXTRA_PKGS
        ;;
    arch|manjaro|endeavouros)
        COMMON_PKGS="git cmake gcc qt5-base ncurses curl nlohmann-json"
        [[ "$DE" == "GNOME" ]] && EXTRA_PKGS="glib2"
        echo "[INFO] Installing for $OS_ID..."
        pacman -S --needed --noconfirm $COMMON_PKGS $EXTRA_PKGS
        ;;
    opensuse-leap|opensuse-tumbleweed)
        COMMON_PKGS="git cmake gcc-c++ libqt5-qtbase-devel ncurses-devel libcurl-devel nlohmann-json-devel"
        [[ "$DE" == "GNOME" ]] && EXTRA_PKGS="glib2-devel"
        echo "[INFO] Installing for $OS_ID..."
        zypper install -y $COMMON_PKGS $EXTRA_PKGS
        ;;
    *)
        echo "[ERROR] Distribution $OS_ID is not supported by this script."
        exit 1
        ;;
esac

echo "--- Summary ---"
echo "Base packages installed: $COMMON_PKGS"
[[ -n "$EXTRA_PKGS" ]] && echo "DE-specific packages installed: $EXTRA_PKGS"
echo "Status: SUCCESS"
