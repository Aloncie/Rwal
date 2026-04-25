#include "menus.hpp"
#include "internal/utils/string_utils.hpp"
#include "menu_ids.hpp"
#include "settings/config.hpp"

// Undefine any existing timeout macro to avoid conflicts with socket options
#ifdef timeout
#undef timeout
#endif

#include <QCoreApplication>
#include <QtConcurrent/QtConcurrent>
#include <QMetaObject>
#include <memory>

namespace MenuId = rwal::ui::MenuId;

// ========== MainMenu ==========
MainMenu::MainMenu(IUserInterface& uim, Keywords& keywords, WallpaperManager& wm, IWallpaperSetter& env, NetworkManager& nm) : m_uim(uim), m_keywords(keywords), m_wm(wm), m_env(env), m_nm(nm) {};
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
MenuResponce MainMenu::handleInput(const std::string& input) {
	if (input == "1") {
		// We don't need to get value from the thread, so we use (void) for tell compiler
		(void)QtConcurrent::run([this] {
			auto error = m_wm.refresh(m_env, m_nm, m_keywords, &m_uim);
			
			QMetaObject::invokeMethod(qApp, [error] {
			}, Qt::QueuedConnection);
    	});
    	return {"", false, false, ""};
    } else if (input == "2") {
        std::string message = m_wm.saveCurrent();
        return {"", false, false, message};
    } else if (input == "3") {
        return {MenuId::KEYWORDS, false, false};
    } else if (input == "4") {
        return {MenuId::SETTINGS, false, false};
    } else if (input == "q") {
        return {"", false, true};
    } else {
        return {"", true, false};
    }
}
// ========== SettingsMenu ==========
SettingsMenu::SettingsMenu(Timer& timer, WallpaperManager& wm, IUserInterface& ui) : m_timer(timer), m_wm(wm), m_uim(ui) {}

std::vector<std::string> SettingsMenu::getLines() {
	auto PicturesPathOpt = m_wm.getPicturesPath(&m_uim);
	std::string pathStr = PicturesPathOpt ? PicturesPathOpt->string() : "Not found";    
	return {
        "--- Settings ---", "1) Timer: " + m_timer.seeTimer(),
        "2) Wallpapers's path: " + pathStr,
		"q) Back",
        ""  // Empty line for spacing
    };
}

MenuResponce SettingsMenu::handleInput(const std::string& input) {
    if (input == "1") {
        return {MenuId::TIMER, false, false};
    } else if (input == "2") {
        // Space for path logic
        return {"", false, false};
    } else if (input == "q") {
        return {MenuId::MAIN, false, false};
    } else {
        return {"", true, false};
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

MenuResponce KeywordsMenu::handleInput(const std::string& input) {
    if (input == "a") {
        m_uim.requestInput([this](std::string keyword) {
			auto keywords = m_keywords.loadKeywordsFromConfig();
            rwal::utils::string::format(keyword);
            keywords.push_back(keyword);
            m_config.set("/search/keywords", keywords);
        });
        return {"", false, false, "Write new keyword: "};
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

        return {"", false, false, "Enter index to remove: "};
    } else if (input == "m") {
        m_keywords.editKeywords(m_uim);
        m_config.reload();
        return {"", false, false};
    } else if (input == "q") {
        return {MenuId::MAIN, false, false};
    } else {
        return {"", true, false};
    }
}

// ========== TimerMenu ==========
TimerMenu::TimerMenu(ISystemScheduler& scheduler) : m_scheduler(scheduler) {}

std::vector<std::string> TimerMenu::getLines() {
    return {
        "(n)one", "(h)ourly", "(d)aily",
        ""  // Empty line for spacing
    };
}

MenuResponce TimerMenu::handleInput(const std::string& input) {
    if (input == "h") {
		std::string_view result = m_timer.set("hourly");
        return {MenuId::SETTINGS, false, false, result};
    } else if (input == "d") {
		std::string_view result = m_timer.set("daily");
        return {MenuId::SETTINGS, false, false, result;};
    } else if (input == "n") {
		std::string_view result = m_timer.set("None");
        return {MenuId::SETTINGS, false, false, result};
    } else {
        return {"", true, false};
    }
}

