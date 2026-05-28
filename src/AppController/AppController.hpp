#pragma once
#include "navigator/navigator.hpp"
#include "ui/tui/menus/menus.hpp"
#include "ui/tui/TUIManager.hpp"

#include <memory>

class AppController {
public:
    AppController(Navigator& nav, TUIManager& tui);
	~AppController() = default;

	bool handleStdin();
private:
    Navigator& m_navigator;
	TUIManager& m_tui;
};

