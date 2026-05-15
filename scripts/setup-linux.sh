# setup-linux.sh - Rwal Linux Dependencies Installer
#!/bin/bash

if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
    echo "[ERROR] This script is for Linux only."
    echo "For Windows, please install dependencies manually:"
    echo "  - Git, CMake, Qt6, curl, nlohmann-json"
    echo "Or use the Windows PowerShell script: ./setup-windows.ps1"
    exit 1
fi

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
elif [ -n "${XDG_CURRENT_DESKTOP}" ]; then
	DE="${XDG_CURRENT_DESKTOP}"
fi

echo "--- Detected System ---"
echo "OS: $OS_ID"
echo "DE: $DE"
echo "-----------------------"

# ============================================================
# Distro-specific package query functions
# ============================================================
check_pkg_debian() { dpkg -s "$1" >/dev/null 2>&1; }
check_pkg_fedora() { rpm -q "$1" >/dev/null 2>&1; }
check_pkg_arch()   { pacman -Qi "$1" >/dev/null 2>&1; }
check_pkg_suse()   { rpm -q "$1" >/dev/null 2>&1; }

# ============================================================
# Deep checkers for tools that need version validation
# These return 0 (found) or 1 (not found)
# ============================================================
check_git()   { command -v git >/dev/null 2>&1; }
check_cmake() { command -v cmake >/dev/null 2>&1; }
check_gxx()   { command -v g++ >/dev/null 2>&1; }
check_qt6() {
    # 1. qtpaths6 (official Qt6 tool)
    if command -v qtpaths6 &>/dev/null; then
        return 0
    elif command -v qtpaths &>/dev/null; then
        if qtpaths --query QT_VERSION 2>/dev/null | grep -q '^6\.'; then
            return 0
        fi
    fi

    # 2. pkg-config
    if pkg-config --exists Qt6Core 2>/dev/null; then
        return 0
    fi

    # 3. Filesystem search in common roots
    local search_roots=("/usr/lib" "/usr/lib64" "/usr/share" "/opt" "$HOME/Qt")
    for root in "${search_roots[@]}"; do
        if [ -d "$root" ]; then
            local found_cmake
            found_cmake=$(find "$root" -maxdepth 5 -path "*/cmake/Qt6/Qt6Config.cmake" -print -quit 2>/dev/null)
            if [ -n "$found_cmake" ]; then
                return 0
            fi
        fi
    done

    return 1
}
check_curl() { command -v curl >/dev/null 2>&1 || pkg-config --exists libcurl 2>/dev/null; }
check_json() { pkg-config --exists nlohmann_json 2>/dev/null; }

# ============================================================
# Pre-check: mark satisfied dependencies so we skip them
# ============================================================
SATISFIED=()

check_git   && SATISFIED+=("git")
check_cmake && SATISFIED+=("cmake")
check_gxx   && SATISFIED+=("g++")
check_qt6   && SATISFIED+=("qt6")
check_curl  && SATISFIED+=("curl")
check_json  && SATISFIED+=("nlohmann-json")

echo "[INFO] Pre-check satisfied: ${SATISFIED[*]:-none}"

# ============================================================
# Distro-specific package lists
# ============================================================
PKGS_NAMES=""
EXTRA_PKGS=""

case "$OS_ID" in
    ubuntu|debian|linuxmint|pop)
        PKG_CHECK="check_pkg_debian"
        PKGS_NAMES="git cmake g++ qtbase6-dev libncurses5-dev libcurl4-openssl-dev libnlohmann-json-dev"
        [[ "$DE" == "GNOME" ]] && EXTRA_PKGS="libglib2.0-dev"
        [[ "$DE" == "Hyprland" ]] && EXTRA_PKGS="hyprpaper swww"
        INSTALL_CMD="apt install -y"
        ;;
    fedora|nobara)
        PKG_CHECK="check_pkg_fedora"
        PKGS_NAMES="git cmake gcc-c++ qt6-qtbase-devel ncurses-devel libcurl-devel nlohmann-json-devel"
        [[ "$DE" == "GNOME" ]] && EXTRA_PKGS="glib2-devel"
        [[ "$DE" == "Hyprland" ]] && EXTRA_PKGS="hyprpaper swww"
        INSTALL_CMD="dnf install -y"
        ;;
    arch|manjaro|endeavouros)
        PKG_CHECK="check_pkg_arch"
        PKGS_NAMES="git cmake gcc qt6-base ncurses curl nlohmann-json"
        [[ "$DE" == "GNOME" ]] && EXTRA_PKGS="glib2"
        [[ "$DE" == "Hyprland" ]] && EXTRA_PKGS="hyprpaper swww"
        INSTALL_CMD="pacman -S --needed --noconfirm"
        ;;
    opensuse-leap|opensuse-tumbleweed*)
        PKG_CHECK="check_pkg_suse"
        PKGS_NAMES="git cmake gcc-c++ libqt6-qtbase-devel ncurses-devel libcurl-devel nlohmann-json-devel"
        [[ "$DE" == "GNOME" ]] && EXTRA_PKGS="glib2-devel"
        [[ "$DE" == "Hyprland" ]] && EXTRA_PKGS="hyprpaper swww"
        INSTALL_CMD="zypper install -y"
        ;;
    *)
        echo "[ERROR] Unknown OS: $OS_ID"
        exit 1
        ;;
esac

# ============================================================
# Check each package, skip if pre-check already satisfied
# ============================================================
MISSING=()

for pkg in $PKGS_NAMES $EXTRA_PKGS; do
    # Skip if deep checker already found this tool
    local skip=false
    for sat in "${SATISFIED[@]}"; do
        if [[ "$pkg" == "$sat" ]]; then
            skip=true
            break
        fi
    done
    if $skip; then
        echo "[OK] $pkg (pre-check)"
        continue
    fi

    # Try executable check, then distro package check
    if command -v "$pkg" >/dev/null 2>&1 || $PKG_CHECK "$pkg"; then
        echo "[OK] $pkg"
    else
        echo "[MISS] $pkg"
        MISSING+=("$pkg")
    fi
done

# ============================================================
# Install only missing
# ============================================================
if [ ${#MISSING[@]} -gt 0 ]; then
    echo ""
    echo "Installing: ${MISSING[*]}"
    $INSTALL_CMD "${MISSING[@]}"
    if [ $? -eq 0 ]; then
        echo ""
        echo "-----------------------"
        echo "STATUS: SUCCESS"
        echo "All dependencies for $DE are installed."
    else
        echo ""
        echo "-----------------------"
        echo "STATUS: FAILED"
        echo "[ERROR] Installation stopped. Check your disk space, internet, or package names."
        exit 1
    fi
else
    echo ""
    echo "-----------------------"
    echo "STATUS: SUCCESS"
    echo "All dependencies already installed. Nothing to do."
fi
