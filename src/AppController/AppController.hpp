#pragma once
#include <QObject>
#include <QSocketNotifier>
#include "navigator/navigator.hpp"
#include "ui/menus/menus.hpp"
#include "ui/cli/UIManager.hpp"

class AppController : public QObject{
	Q_OBJECT;
public:
	AppController(Navigator* nav, QObject* parent = nullptr);
private slots:
	void handleStdin(UIManager uimanager);
private:
	QSocketNotifier* m_notifier;
	Navigator* m_navigator;
};

