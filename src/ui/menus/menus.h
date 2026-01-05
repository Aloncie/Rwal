#pragma once
#include <vector>
#include <string>
#include <functional>
#include "ui/io/io.h"

namespace rwal::ui {

    struct BaseMenu;

    struct MenuResponce {
        BaseMenu* nextMenu;
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

    struct BaseMenu {
        virtual ~BaseMenu() = default;
        virtual std::vector<std::string> menu() const = 0;
        virtual std::string get_input() const = 0;
        virtual MenuResponce execute_actions(std::string input) const = 0;
    };

    struct CharacterMenuConfig : public BaseMenu {
        const std::string valid_choices;
        std::function<std::vector<std::string>()> menu_generator;
        std::function<MenuResponce(std::string)> logic_handler;

        CharacterMenuConfig(
            std::string choices,
            std::function<std::vector<std::string>()> gen,
            std::function<MenuResponce(std::string)> logic
        ) : valid_choices(choices), menu_generator(gen), logic_handler(logic) {}

        MenuResponce execute_actions(std::string input) const override {
            return logic_handler(input);
        }
        
        std::vector<std::string> menu() const override { return menu_generator(); }
        std::string get_input() const override;
    };

    struct ArrowMenuConfig : public BaseMenu {
        std::function<std::vector<std::string>()> menu_generator;
        std::function<MenuResponce(std::string)> logic_handler;

        ArrowMenuConfig(
            std::function<std::vector<std::string>()> gen,
            std::function<MenuResponce(std::string)> logic
        ) : menu_generator(gen), logic_handler(logic) {}

        MenuResponce execute_actions(std::string input) const override {
            return logic_handler(input);
        }

        std::vector<std::string> menu() const override { return menu_generator(); }
        std::string get_input() const override;
    };
	extern const rwal::ui::CharacterMenuConfig MAIN_MENU;
	extern const rwal::ui::CharacterMenuConfig KEYWORDS_MENU;
	extern const rwal::ui::CharacterMenuConfig SETTINGS_MENU;
	extern const rwal::ui::ArrowMenuConfig TIMER_MENU;
}


