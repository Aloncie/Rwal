#pragma once
#include <vector>
#include <string>
#include <functional>
#include "ui/cli/UIManager.hpp"
#include "keywords/keywords.hpp"
#include "settings/settings.hpp"

struct MenuResponce {
    std::string nextMenu;
    bool IsWrongInput;    
    bool needQuit;
};

class Menu{
public:	
    virtual ~Menu() = default;
    virtual std::vector<std::string> getLines() = 0;
    virtual MenuResponce handleInput(const std::string& input) = 0;
    virtual std::string getValidChoices() const = 0;
};

class MainMenu : public Menu{
private:
    UIManager& m_ui;
    Keywords& m_keywords;
    const std::string m_validChoices = "1234q";
public:
    MainMenu(UIManager& ui, Keywords& keywords);
    std::vector<std::string> getLines() override;
    MenuResponce handleInput(const std::string& input) override;
    std::string getValidChoices() const override { return m_validChoices; }
};

class SettingsMenu : public Menu{
private:
    Timer& m_timer;
    const std::string m_validChoices = "12q";
public:
    SettingsMenu(Timer& timer);
    std::vector<std::string> getLines() override;
    MenuResponce handleInput(const std::string& input) override;
    std::string getValidChoices() const override { return m_validChoices; }
};

class KeywordsMenu : public Menu{
private:
    Keywords& m_keywords;
    UIManager& m_ui;
    const std::string m_validChoices = "armq";  
public:
    KeywordsMenu(Keywords& keywords, UIManager& ui);
    std::vector<std::string> getLines() override;
    MenuResponce handleInput(const std::string& input) override;
    std::string getValidChoices() const override { return m_validChoices; }
};

class TimerMenu : public Menu{
private:
    Timer& m_timer;
    const std::string m_validChoices = "nhd";
public:
    TimerMenu(Timer& timer);
    std::vector<std::string> getLines() override;
    MenuResponce handleInput(const std::string& input) override;
    std::string getValidChoices() const override { return m_validChoices; }
};
