#pragma once
#include <string>
#include <vector>

#include "keywords/keywords.hpp"
#include "settings/config.hpp"
#include "settings/settings.hpp"
#include "ui/cli/UIManager.hpp"
#include "wallpaper/WallpaperManager.hpp"

struct MenuResponce {
    std::string nextMenu;
    bool IsWrongInput;
    bool needQuit;
    std::string Message;
};

class Menu {
public:
    virtual ~Menu() = default;
    virtual std::vector<std::string> getLines() = 0;
    virtual MenuResponce handleInput(const std::string& input) = 0;
    virtual std::string getValidChoices() const = 0;
};

class MainMenu : public Menu {
private:
    UIManager& m_ui;
    Keywords& m_keywords;
    WallpaperManager& m_wm;
    const std::string m_validChoices = "1234q";

public:
    MainMenu(UIManager& ui, Keywords& keywords, WallpaperManager& wm);
    std::vector<std::string> getLines() override;
    MenuResponce handleInput(const std::string& input) override;
    std::string getValidChoices() const override { return m_validChoices; }
};

class SettingsMenu : public Menu {
private:
    Timer& m_timer;
    WallpaperManager& m_wm;
    const std::string m_validChoices = "12q";

public:
    SettingsMenu(Timer& timer, WallpaperManager& wm);
    std::vector<std::string> getLines() override;
    MenuResponce handleInput(const std::string& input) override;
    std::string getValidChoices() const override { return m_validChoices; }
};

class KeywordsMenu : public Menu {
private:
    Keywords& m_keywords;
    UIManager& m_ui;
    Config& m_config;
    const std::string m_validChoices = "armq";

public:
    KeywordsMenu(Keywords& keywords, UIManager& ui, Config& config);
    std::vector<std::string> getLines() override;
    MenuResponce handleInput(const std::string& input) override;
    std::string getValidChoices() const override { return m_validChoices; }
};

class TimerMenu : public Menu {
private:
    Timer& m_timer;
    const std::string m_validChoices = "nhd";

public:
    TimerMenu(Timer& timer);
    std::vector<std::string> getLines() override;
    MenuResponce handleInput(const std::string& input) override;
    std::string getValidChoices() const override { return m_validChoices; }
};
