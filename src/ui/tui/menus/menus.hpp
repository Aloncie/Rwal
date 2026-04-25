#pragma once
#include "keywords/keywords.hpp"
#include "settings/settings.hpp"
#include "settings/ISystemScheduler.hpp"
#include "settings/IConfigReader.hpp"
#include "ui/IUserInterface.hpp"
#include "wallpaper/WallpaperManager.hpp"
#include "IWallpaperSetter.hpp"
#include "net/NetworkManager.hpp"

#include <string>
#include <vector>

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
    virtual const std::string& getValidChoices() const = 0;
};

class MainMenu : public Menu {
private:
    IUserInterface& m_uim;
    Keywords& m_keywords;
    WallpaperManager& m_wm;
	IWallpaperSetter& m_env;
	NetworkManager& m_nm;
    inline static const std::string m_validChoices = "1234q";

public:
    MainMenu(IUserInterface& uim, Keywords& keywords, WallpaperManager& wm, IWallpaperSetter& env, NetworkManager& nm);
    std::vector<std::string> getLines() override;
    MenuResponce handleInput(const std::string& input) override;
    const std::string& getValidChoices() const override { return m_validChoices; }
};

class SettingsMenu : public Menu {
private:
    ISystemScheduler& m_scheduler;
    WallpaperManager& m_wm;
	IUserInterface& m_uim;
    inline static const std::string m_validChoices = "12q";

public:
    SettingsMenu(ISystemScheduler& scheduler, WallpaperManager& wm, IUserInterface& ui);
    std::vector<std::string> getLines() override;
    MenuResponce handleInput(const std::string& input) override;
    const std::string& getValidChoices() const override { return m_validChoices; }
};

class KeywordsMenu : public Menu {
private:
    Keywords& m_keywords;
    IUserInterface& m_uim;
	IConfigReader& m_config;
    inline static const std::string m_validChoices = "armq";

public:
    KeywordsMenu(Keywords& keywords, IUserInterface& ui, IConfigReader& config);
    std::vector<std::string> getLines() override;
    MenuResponce handleInput(const std::string& input) override;
    const std::string& getValidChoices() const override { return m_validChoices; }
};

class SchedulerMenu : public Menu {
private:
    ISystemScheduler& m_scheduler;
    inline static const std::string m_validChoices = "nhd";

public:
    SchedulerMenu(ISystemScheduler& scheduler);
    std::vector<std::string> getLines() override;
    MenuResponce handleInput(const std::string& input) override;
    const std::string& getValidChoices() const override { return m_validChoices; }
};

