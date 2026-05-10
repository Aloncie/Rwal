#pragma once

#include <string_view>
#include <string>

namespace rwal::constants {

    namespace files {
        // core
        inline constexpr std::string_view CONFIG_FILE = "config.json";
        inline constexpr std::string_view LOGS_FILE = "logs.txt";
        inline constexpr std::string_view KEYWORDS_TEMP = "keywords.txt";

        // linux scheduler
        inline constexpr std::string_view SERVICE_FILE = "rwal.service";
        inline constexpr std::string_view TIMER_FILE = "rwal.timer";

        // windows scheduler
		// use wstring instead of wstring_view to avoid not null-terminated.
        inline const std::wstring EXEC_FILE = L"rwal.exe";
    }

    namespace names {
        inline constexpr std::string_view SERVICE_NAME = "rwal";
        inline const std::wstring WIN_TASK_NAME = L"Rwal Wallpaper Changer";
    }

    namespace dirs {
        inline constexpr std::string_view LINUX_SYSTEMD_USER = ".config/systemd/user";
        inline constexpr std::string_view DOWNLOADS_DIR = "downloads";
    }

    namespace wallpaper {
        inline constexpr std::string_view FILE_PREFIX = "wallpaper-";
        inline constexpr int SUFFIX_LENGTH = 6;
        inline constexpr std::string_view DEFAULT_EXT = ".jpg";
    }

    namespace systemd {
        inline constexpr std::string_view SUPPRESS_OUTPUT = " >/dev/null 2>&1";
    }
}

