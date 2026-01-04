#pragma once
#include <map>
#include <vector>
#include <string>
#include <functional>
#include "ui/io/io.h"


namespace rwal::ui {
	namespace generators {
		std::vector<std::string> keywords_list();
		std::vector<std::string> settings_list();
	}
	namespace ready {
		const std::vector<std::string> main_list();
		const std::vector<std::string> timer_list();
	}
	struct BaseMenu{
		virtual ~BaseMenu() = default;
		virtual std::vector<std::string> menu() const = 0;
		virtual std::string get_input() const = 0;
		virtual BaseMenu* execute_actions(std::string input) const = 0;
	};

	struct CharacterMenuConfig : public BaseMenu {
		const std::string valid_choices;
		std::function<std::vector<std::string>()> menu_generator;
		std::map<char, std::function<void()>> actions;

		std::vector<std::string> menu() const override { return menu_generator(); }
		std::string get_input() const override;
	};

	struct ArrowMenuConfig : public BaseMenu {
		std::function<std::vector<std::string>()> menu_generator;
		void execute_actions(char c) const override{};
		std::vector<std::string> menu() const override { return menu_generator(); }
		std::string get_input() const override;
	};

}

extern const rwal::ui::CharacterMenuConfig MAIN_MENU;
extern const rwal::ui::CharacterMenuConfig KEYWORDS_MENU; 
extern const rwal::ui::CharacterMenuConfig SETTINGS_MENU;

extern const rwal::ui::ArrowMenuConfig TIMER_MENU;
