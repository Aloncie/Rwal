#pragma once
#include "navigator/navigator.hpp"
#include "ui/tui/menus/menus.hpp"
#include "ui/tui/TUIManager.hpp"

#include <memory>


class AppController : public QObject {
    Q_OBJECT;
public:
    AppController(Navigator& nav, TUIManager& ui, QObject* parent = nullptr);
	~AppController() override = default;
private slots:
    void handleStdin();
private:
    Navigator& m_navigator;
	TUIManager& m_ui;
    std::unique_ptr<QSocketNotifier> m_notifier;
};

