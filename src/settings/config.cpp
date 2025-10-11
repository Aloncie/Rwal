#include "config.h"
#include <QDir>
#include <QCoreApplication>
#include <QStandardPaths>

QString Config::getConfigPath(){
	QCoreApplication::setApplicationName("rwal");
	QCoreApplication::setOrganizationName("Aloncie");

	QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

	QDir dir;

	if (!dir.exists(configDir))
		dir.mkpath(configDir);

	return configDir + "/config.json";
}

Config::Config(){
	QString cfg = getConfigPath();		
}
