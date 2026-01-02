#pragma once
#include <map>
#include <optional>
#include <vector>
#include <string>
#include <functional>
#include "ui/io/io.h"

struct BaseMenu{
	virtual ~BaseMenu() = default;
	virtual std::vector<std::string> menu() const = 0;
	virtual char get_input() const = 0;	
	virtual void execute_action(char c) const = 0;
};

struct CharacterMenuConfig : public BaseMenu {
    const std::string valid_choices;
    std::function<std::vector<std::string>()> menu_generator;
    std::map<char, std::function<void()>> execute_actions;

    std::vector<std::string> menu() const override { return menu_generator(); }
    char get_input() const override { 
        return rwal::ui::io::characterInput();
    }
};

struct ArrowMenuConfig : public BaseMenu {
    std::function<std::vector<std::string>()> menu_generator;

    std::vector<std::string> menu() const override { return menu_generator(); }
    char get_input() const override { 
        return rwal::ui::io::arrowInput();
    }
};

namespace rwal::ui {
	namespace generators {
		std::vector<std::string> keywords_list();
		std::vector<std::string> settings_list();
	}
	namespace ready {
		const std::vector<std::string> main_list();
		const std::vector<std::string> timer_list();
	}
}

extern const CharacterMenuConfig MAIN_MENU;
extern const CharacterMenuConfig KEYWORDS_MENU; 
extern const CharacterMenuConfig SETTINGS_MENU;

extern const ArrowMenuConfig TIMER_MENU;
