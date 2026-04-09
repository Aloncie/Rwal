#pragma once
#include "navigator/navigator.hpp"
#include "ui/menus/menus.hpp"
#include "ui/tui/TUIManager.hpp"

#include <QObject>
#include <QSocketNotifier>

class AppController : public QObject {
    Q_OBJECT;

public:
    AppController(Navigator* nav, TUIManager& ui, QObject* parent = nullptr);
private slots:
    void handleStdin();

private:
    QSocketNotifier* m_notifier;
    Navigator* m_navigator;
    TUIManager& m_ui;
};

