#include "KdeSetter.hpp"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QVariantMap>
#include <QDebug>
#include <QString>

bool KdeSetter::setWallpaper(const fs::path& path) {
    QString hostPathUri = "file://" + QString::fromStdString(path);

    QString script = QString(
        "var allDesktops = desktops();"
        "for (var i = 0; i < allDesktops.length; i++) {"
        "    allDesktops[i].wallpaperPlugin = 'org.kde.image';"
        "    allDesktops[i].currentConfigGroup = ['Wallpaper', 'org.kde.image', 'General'];"
        "    allDesktops[i].writeConfig('Image', '%1');"
        "}"
    ).arg(hostPathUri);

    auto msg = QDBusMessage::createMethodCall(
        QStringLiteral("org.kde.plasmashell"),
        QStringLiteral("/PlasmaShell"),
        QStringLiteral("org.kde.PlasmaShell"),
        QStringLiteral("evaluateScript")
    );

    msg << script;

    auto reply = QDBusConnection::sessionBus().call(msg);
    
    if (reply.type() == QDBusMessage::ErrorMessage) {
        m_logs.writeLogs(rwal::logs::types::Error, rwal::logs::modules::Wallpaper, "D-Bus Error: " + reply.errorMessage().toStdString());
		return false;
    } else {
        m_logs.writeLogs(rwal::logs::types::Info, rwal::logs::modules::Wallpaper, "Wallpaper change signal sent for: " + hostPathUri.toStdString());
		return true;
    }
}


