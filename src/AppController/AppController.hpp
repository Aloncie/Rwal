#pragma once
#include <QObject>
#include <QSocketNotifier>
#include "navigator/navigator.hpp"
#include "ui/menus/menus.hpp"

class AppController : public QObject{
	Q_OBJECT;
public:
	AppController(Navigator* nav, QObject* parent = nullptr);
private slots:
	void handleStdin();
private:
	QSocketNotifier* m_notifier;
	Navigator* m_navigator;
};

