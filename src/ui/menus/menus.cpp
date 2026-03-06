#include "menus.hpp"
#include "settings/config.hpp"
#include "internal/utils/string_utils.hpp"
#include "wallpaper/WallpaperManager.hpp"
#include "menu_ids.hpp"
#include <QCoreApplication>

namespace MenuId = rwal::ui::MenuId;

// ========== MainMenu ==========
MainMenu::MainMenu(UIManager& ui, Keywords& keywords) 
    : m_ui(ui), m_keywords(keywords) {}

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
        refresh_wallpaper(m_keywords);
        return {"", false, false};
    } 
    else if (input == "2") {
        save_wallpaper(where_are_wallpaper(), m_ui);
        return {"", false, false};
    } 
    else if (input == "3") {
        return {MenuId::KEYWORDS, false, false};
    } 
    else if (input == "4") {
        return {MenuId::SETTINGS, false, false};
    } 
    else if (input == "q") {
        return {"", false, true};
    } 
    else {
        return {"", true, false};
    }
}

// ========== SettingsMenu ==========
SettingsMenu::SettingsMenu(Timer& timer) 
    : m_timer(timer) {}

std::vector<std::string> SettingsMenu::getLines() {
    return {
        "--- Settings ---",
        "1) Timer: " + m_timer.see_timer(),
        "2) Wallpapers's path: " + where_are_wallpaper(),
        "q) Back",
        ""  // Empty line for spacing
    };
}

MenuResponce SettingsMenu::handleInput(const std::string& input) {
    if (input == "1") {
        return {MenuId::TIMER, false, false};
    } 
    else if (input == "2") {
        // Space for path logic
        return {"", false, false};
    } 
    else if (input == "q") {
        return {MenuId::MAIN, false, false};
    } 
    else {
        return {"", true, false};
    }
}

// ========== KeywordsMenu ==========
KeywordsMenu::KeywordsMenu(Keywords& keywords, UIManager& ui)
    : m_keywords(keywords), m_ui(ui) {}

std::vector<std::string> KeywordsMenu::getLines() {
    std::vector<std::string> lines = {"--- Keywords Editor ---"};
    auto keywords = m_keywords.ShortWayGetKeywords<std::vector<std::string>>();
    
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
    auto keywords = m_keywords.ShortWayGetKeywords<std::vector<std::string>>();

    if (input == "a") {
        std::string keyword = m_ui.requestInput<std::string>("Write new keyword: ");
        if (!keyword.empty()) {
            rwal::utils::string::format(keyword);
            keywords.push_back(keyword);
            Config::getInstance().set("/search/keywords", keywords);
        }
        return {"", false, false};
    } 
    else if (input == "r") {
        int display_index = m_ui.requestInput<int>("Enter index for remove: ");
        if (display_index >= 1) {
            int real_index = display_index - 1;
            if (real_index < (int)keywords.size()) {
                keywords.erase(keywords.begin() + real_index);
                Config::getInstance().set("/search/keywords", keywords);
            }
        }
        return {"", false, false};
    } 
    else if (input == "m") {
        m_keywords.editKeywords();
        Config::getInstance().loadConfig();
        return {"", false, false};
    } 
    else if (input == "q") {
        return {MenuId::MAIN, false, false};
    } 
    else {
        return {"", true, false};
    }
}

// ========== TimerMenu ==========
TimerMenu::TimerMenu(Timer& timer) : m_timer(timer) {}

std::vector<std::string> TimerMenu::getLines() {
    return {
        "(n)one",
        "(h)ourly",
        "(d)aily",
        ""  // Empty line for spacing
    };
}

MenuResponce TimerMenu::handleInput(const std::string& input) {
    if (input == "h") {
        m_timer.edit_timer("hourly");
        return {MenuId::SETTINGS, false, false};
    } 
    else if (input == "d") {
        m_timer.edit_timer("daily");
        return {MenuId::SETTINGS, false, false};
    } 
    else if (input == "n") {
        m_timer.edit_timer("None");
        return {MenuId::SETTINGS, false, false};
    } 
    else {
        return {"", true, false};
    }
}
