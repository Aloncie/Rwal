#!/bin/bas#

if [[ $EUID -ne 0 ]]; then
   echo "[ERROR] Please run this script with sudo."
   exit 1
fi

if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS_ID=$ID
else
    echo "[ERROR] Cannot detect Operating System."
    exit 1
fi

DE="Unknown"
if pgrep -x "gnome-shell" > /dev/null; then
    DE="GNOME"
elif pgrep -x "plasmashell" > /dev/null; then
    DE="KDE"
elif pgrep -x "Hyprland" > /dev/null; then
    DE="Hyprland"
fi

echo "--- Detected System ---"
echo "OS: $OS_ID"
echo "DE: $DE"
echo "-----------------------"

COMMON_PKGS=""
EXTRA_PKGS=""

case "$OS_ID" in
    ubuntu|debian|linuxmint|pop)
        COMMON_PKGS="git cmake g++ qtbase5-dev libncurses5-dev libcurl4-openssl-dev libnlohmann-json-dev"
        [[ "$DE" == "GNOME" ]] && EXTRA_PKGS="libglib2.0-dev"
        [[ "$DE" == "Hyprland" ]] && EXTRA_PKGS="hyprpaper swww"
        INSTALL_CMD="apt update && apt install -y"
        ;;
    fedora|nobara)
        COMMON_PKGS="git cmake gcc-c++ qt5-qtbase-devel ncurses-devel libcurl-devel nlohmann-json-devel"
        [[ "$DE" == "GNOME" ]] && EXTRA_PKGS="glib2-devel"
        [[ "$DE" == "Hyprland" ]] && EXTRA_PKGS="hyprpaper swww"
        INSTALL_CMD="dnf install -y"
        ;;
    arch|manjaro|endeavouros)
        COMMON_PKGS="git cmake gcc qt5-base ncurses curl nlohmann-json"
        [[ "$DE" == "GNOME" ]] && EXTRA_PKGS="glib2"
        [[ "$DE" == "Hyprland" ]] && EXTRA_PKGS="hyprpaper swww"
        INSTALL_CMD="pacman -S --needed --noconfirm"
        ;;
    opensuse-leap|opensuse-tumbleweed)
        COMMON_PKGS="git cmake gcc-c++ libqt5-qtbase-devel ncurses-devel libcurl-devel nlohmann-json-devel"
        [[ "$DE" == "GNOME" ]] && EXTRA_PKGS="glib2-devel"
        [[ "$DE" == "Hyprland" ]] && EXTRA_PKGS="hyprpaper swww"
        INSTALL_CMD="zypper install -y"
        ;;
esac

echo "[INFO] Installing: $COMMON_PKGS $EXTRA_PKGS"
$INSTALL_CMD $COMMON_PKGS $EXTRA_PKGS

if [ $? -eq 0 ]; then
    echo "-----------------------"
    echo "STATUS: SUCCESS"
    echo "All dependencies for $DE are installed."
else
    echo "-----------------------"
    echo "STATUS: FAILED"
	echo "[ERROR] Installation stopped. Check your disk space, internet or smth else."
    exit 1
fi
