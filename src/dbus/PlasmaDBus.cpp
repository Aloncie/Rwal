#include "PlasmaDBus.hpp"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QVariantMap>
#include <QDebug>
#include <QString>
#include "logs/logs.hpp"

void change_wallpaper(const std::string& imagePath) {
    std::string resolvedPath = imagePath;
    
    const char* container_home = std::getenv("HOME");
    const char* host_home = std::getenv("HOST_HOME");

    if (container_home && host_home) {
        std::string c_home(container_home);
        if (resolvedPath.find(c_home) == 0) {
            resolvedPath.replace(0, c_home.length(), std::string(host_home));
        }
    }

    QString hostPathUri = "file://" + QString::fromStdString(resolvedPath);

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
        Logs::getInstance().writeLogs("D-Bus Error: " + reply.errorMessage().toStdString());
    } else {
        Logs::getInstance().writeLogs("Wallpaper change signal sent for: " + resolvedPath);
    }
}
