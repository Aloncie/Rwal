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

namespace MenuId = rwal::ui::MenuId;

// ========== MainMenu ==========
MainMenu::MainMenu(UIManager& ui, Keywords& keywords, WallpaperManager& wm, IWallpaperSetter& env, NetworkManager& nm) : m_ui(ui), m_keywords(keywords), m_wm(wm), m_env(env), m_nm(nm) {};
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
		QtConcurrent::run([this] {
			m_wm.refresh(m_env, m_nm, m_keywords, &m_ui);
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
SettingsMenu::SettingsMenu(Timer& timer, WallpaperManager& wm, UIManager& ui) : m_timer(timer), m_wm(wm), m_ui(ui) {}

std::vector<std::string> SettingsMenu::getLines() {
	auto PicturesPathOpt = m_wm.getPicturesPath(&m_ui);
	if (!PicturesPathOpt) PicturesPathOpt = "Not found";
    return {
        "--- Settings ---", "1) Timer: " + m_timer.seeTimer(),
        "2) Wallpapers's path: " + PicturesPathOpt->string(),
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
KeywordsMenu::KeywordsMenu(Keywords& keywords, UIManager& ui, IConfigReader& config)
    : m_keywords(keywords), m_ui(ui), m_config(config) {}

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
        m_ui.requestInputString([this](std::string keyword) {
			auto keywords = m_keywords.loadKeywordsFromConfig();
            rwal::utils::string::format(keyword);
            keywords.push_back(keyword);
            m_config.set("/search/keywords", keywords);
        });
        return {"", false, false, "Write new keyword: "};
    } else if (input == "r") {
        m_ui.requestInputInt([this](int display_index) {
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
        m_keywords.editKeywords(m_ui);
        m_config.reload();
        return {"", false, false};
    } else if (input == "q") {
        return {MenuId::MAIN, false, false};
    } else {
        return {"", true, false};
    }
}

// ========== TimerMenu ==========
TimerMenu::TimerMenu(Timer& timer) : m_timer(timer) {}

std::vector<std::string> TimerMenu::getLines() {
    return {
        "(n)one", "(h)ourly", "(d)aily",
        ""  // Empty line for spacing
    };
}

MenuResponce TimerMenu::handleInput(const std::string& input) {
    if (input == "h") {
        m_timer.editTimer("hourly");
        return {MenuId::SETTINGS, false, false};
    } else if (input == "d") {
        m_timer.editTimer("daily");
        return {MenuId::SETTINGS, false, false};
    } else if (input == "n") {
        m_timer.editTimer("None");
        return {MenuId::SETTINGS, false, false};
    } else {
        return {"", true, false};
    }
}

