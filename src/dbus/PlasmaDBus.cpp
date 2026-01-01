#include "PlasmaDBus.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QVariantMap>
#include <QDebug>
#include <QString>
#include "logs/logs.h"

void change_wallpaper(std::string local) {
    QString path = "file://" + QString::fromStdString(local);

    QString script = QString(
        "var allDesktops = desktops();"
        "for (var i = 0; i < allDesktops.length; i++) {"
        "    allDesktops[i].wallpaperPlugin = 'org.kde.image';"
        "    allDesktops[i].currentConfigGroup = Array('Wallpaper', 'org.kde.image', 'General');"
        "    allDesktops[i].writeConfig('Image', '%1');"
        "}"
    ).arg(path);

    auto msg = QDBusMessage::createMethodCall(
        QStringLiteral("org.kde.plasmashell"),
        QStringLiteral("/PlasmaShell"),
        QStringLiteral("org.kde.PlasmaShell"),
        QStringLiteral("evaluateScript")
    );

    msg << script;

    auto reply = QDBusConnection::sessionBus().call(msg);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        Logs::getInstance().write_logs("D-Bus Error: " + reply.errorMessage().toStdString());
    }
}
