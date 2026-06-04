#include "menus.hpp"
#include "internal/utils/string_utils.hpp"
#include "menu_ids.hpp"
#include "settings/config.hpp"
#include "settings/SchedulerTypes.hpp"
#include "AppConfig.h"

#include <memory>
#include <thread>

namespace MenuId = rwal::ui::MenuId;

// ========== MainMenu ==========
MainMenu::MainMenu(IUserInterface& uim, Keywords& keywords, WallpaperManager& wm, IWallpaperSetter& env, NetworkManager& netmanager) : m_uim(uim), m_keywords(keywords), m_wm(wm), m_env(env), m_netmanager(netmanager) {};
std::vector<std::string> MainMenu::getLines() {
    return {
        "--- Main Menu ---",
        "1) Refresh",
        "2) Save",
        "3) Keywords",
        "4) Settings",
        "q) Quit",
        ""  // Empty line for spacing
    };
}
MenuResponse MainMenu::handleInput(const std::string& input) {
	if (input == "1") {
    	return {"", "", false, false, true};
    } else if (input == "2") {
        std::string message = m_wm.saveCurrent();
        return {"", message};
    } else if (input == "3") {
        return {MenuId::KEYWORDS};
    } else if (input == "4") {
        return {MenuId::SETTINGS};
    } else if (input == "q") {
        return {"", "", false, true};
    } else {
        return {""};
    }
}
// ========== SettingsMenu ==========
SettingsMenu::SettingsMenu(ISystemScheduler& scheduler, WallpaperManager& wm, IUserInterface& ui, IFileSystem& fs) : m_scheduler(scheduler), m_wm(wm), m_uim(ui), m_fs(fs) {}

std::vector<std::string> SettingsMenu::getLines() {
	std::string pathStr = (m_fs.getPicturesLocation() / APP_NAME).string();
	if (pathStr.empty()) pathStr = "Not found";
	return {
        "--- Settings ---", 
		"1) Scheduler: " + m_scheduler.get(),
        "2) Wallpapers's path: " + pathStr,
		"q) Back",
        ""  // Empty line for spacing
    };
}

MenuResponse SettingsMenu::handleInput(const std::string& input) {
    if (input == "1") {
        return {MenuId::SCHEDULER};
    } else if (input == "2") {
        // Space for path logic
        return {""};
    } else if (input == "q") {
        return {MenuId::MAIN};
    } else {
        return {"", "", true};
    }
}

// ========== KeywordsMenu ==========
KeywordsMenu::KeywordsMenu(Keywords& keywords, IUserInterface& ui, IConfigReader& config)
    : m_keywords(keywords), m_uim(ui), m_config(config) {}

std::vector<std::string> KeywordsMenu::getLines() {
    std::vector<std::string> lines = {"--- Keywords Editor ---"};
	auto keywords = m_keywords.loadKeywordsFromConfig();

    if (keywords.empty()) {
        lines.push_back("None");
    } else {
        for (size_t i = 0; i < keywords.size(); ++i) {
            lines.push_back(std::to_string(i + 1) + ". " + keywords[i]);
        }
    }
    lines.push_back("a) Add | r) Remove | m) Manual(editor) | q) Back");
    lines.push_back("");  // Empty line for spacing
    return lines;
}

MenuResponse KeywordsMenu::handleInput(const std::string& input) {
    if (input == "a") {
        m_uim.requestInput([this](std::string keyword) {
			auto keywords = m_keywords.loadKeywordsFromConfig();
            rwal::utils::string::format(keyword);
            keywords.push_back(keyword);
            m_config.set("/search/keywords", keywords);
        });
        return {"", "Write new keyword: "};
    } else if (input == "r") {
        m_uim.requestInput([this](std::string indexStr) {
			int display_index = std::stoi(indexStr);
			auto keywords = m_keywords.loadKeywordsFromConfig();
			if (display_index >= 1) {
				int real_index = display_index - 1;
				if (real_index < (int)keywords.size()) {
					keywords.erase(keywords.begin() + real_index);
					m_config.set("/search/keywords", keywords);
				}
			}
        });

        return {"", "Enter index to remove: "};
    } else if (input == "m") {
        m_keywords.editKeywords(m_uim);
        m_config.reload();
        return {""};
    } else if (input == "q") {
        return {MenuId::MAIN};
    } else {
        return {"", "", true};
    }
}

// ========== Scheduler Menu ==========
SchedulerMenu::SchedulerMenu(ISystemScheduler& scheduler) : m_scheduler(scheduler) {}

std::vector<std::string> SchedulerMenu::getLines() {
	using namespace rwal::system::Scheduler;
    return {
		toString(TaskSchedulerType::None), toString(TaskSchedulerType::Hourly), toString(TaskSchedulerType::Daily),
        ""  // Empty line for spacing
    };
}

MenuResponse SchedulerMenu::handleInput(const std::string& input) {
    if (input == "h") {
		std::string result = m_scheduler.set("hourly");
        return {MenuId::SETTINGS, result};
    } else if (input == "d") {
		std::string result = m_scheduler.set("daily");
        return {MenuId::SETTINGS, result};
    } else if (input == "n") {
		std::string result = m_scheduler.set("None");
        return {MenuId::SETTINGS, result};
    } else {
        return {"", "", true};
    }
}

