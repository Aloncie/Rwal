#pragma once
#include <vector>
#include <string>
#include <functional>

namespace rwal::ui {
	struct CharacterMenuConfig;

    struct MenuResponce {
		const CharacterMenuConfig* nextMenu;
        bool IsWrongInput;    
    };

    namespace generators {
        std::vector<std::string> keywords_list();
        std::vector<std::string> settings_list();
    }

    namespace ready {
        std::vector<std::string> main_list();
        std::vector<std::string> timer_list();
    }

    struct CharacterMenuConfig{
        const std::string valid_choices;
        std::function<std::vector<std::string>()> menu_generator;
        std::function<MenuResponce(std::string)> logic_handler;

        CharacterMenuConfig(
            std::string choices,
            std::function<std::vector<std::string>()> gen,
            std::function<MenuResponce(std::string)> logic
        ) : valid_choices(choices), menu_generator(gen), logic_handler(logic) {}

        MenuResponce execute_actions(std::string input) const {
            return logic_handler(input);
        }
        
        std::vector<std::string> menu() const { return menu_generator(); }
    };

	extern const CharacterMenuConfig MAIN_MENU;
	extern const CharacterMenuConfig KEYWORDS_MENU;
	extern const CharacterMenuConfig SETTINGS_MENU;
	extern const CharacterMenuConfig TIMER_MENU;
}


