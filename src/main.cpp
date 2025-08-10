#include <string>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QVariantMap>
#include <QDebug>
#include "net/CurlWrapper.h"

class Settings{
	void see_settings();
	void edit_keywords(); //Make with setenv("EDITOR", "/usr/bin/vim", 1);
	void edit_timer();
};

int main(int argc, char *argv[]){
	std::string url;
	std::string local;
	auto keywords = divide_keywords(get_keywords());

	do {
		std::string kw = choose_keyword(keywords);
		MyCurl c("an1CFSaR5hyU5D5AM7lCl66FCzp9Dp4a", kw);
		c.get_request();
		url = c.get_image_url();
		if (!url.empty()) {
			local = c.download_image(url);
			break;
		}
	} while (true);	

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
		qWarning() << "Error D-Bus: " << reply.errorMessage();
	}

	return 0;
}
