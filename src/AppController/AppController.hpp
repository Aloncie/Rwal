#pragma once
#include "navigator/navigator.hpp"
#include "ui/tui/menus/menus.hpp"
#include "ui/tui/TUIManager.hpp"

#include <memory>
#include <QObject>

#ifdef Q_OS_UNIX
	#include <QSocketNotifier>
#elif defined(Q_OS_WIN)
	#include <QWinEventNotifier>
	#include <windows.h>
#else
	#warning "Unknown OS in AppController's detection"
#endif

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
#ifdef Q_OS_UNIX
    std::unique_ptr<QSocketNotifier> m_notifier;
#else
    std::unique_ptr<QWinEventNotifier> m_notifier;
#endif
};

