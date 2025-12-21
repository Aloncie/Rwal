#include "PlasmaDBus.h"
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QVariantMap>
#include <QDebug>
#include <QString>
#include "logs/logs.h"
#include "CLI/CLI.h"

void change_wallpaper(int argc, char** argv,std::string local){

	QCoreApplication app(argc, argv);

	QString local_q = QString::fromStdString(local);
	
	QVariantMap params;	
	params.insert(QStringLiteral("Image"), local_q);

	auto msg = QDBusMessage::createMethodCall(
		QStringLiteral("org.kde.plasmashell"),
		QStringLiteral("/PlasmaShell"),
		QStringLiteral("org.kde.PlasmaShell"),
		QStringLiteral("setWallpaper")	
	);
	msg << QStringLiteral("org.kde.image")
		<< params
		<< quint32(0);

	auto reply = QDBusConnection::sessionBus().call(msg);
	if (reply.type() == QDBusMessage::ErrorMessage) {
		 
		Logs::getInstance().write_logs("Error D-Bus");
		MenuManager::getInstatce().show_message("Failed of changing wallpaper");
	}

}
