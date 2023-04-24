#include "desktopsettings.h"

#include <QDBusServiceWatcher>
#include <QProcess>

DesktopSettings::DesktopSettings(QObject *parent)
    : QObject(parent)
    , m_interface("com.cute.Settings",
                  "/Theme", "com.cute.Theme",
                  QDBusConnection::sessionBus(), this)
{
    QDBusServiceWatcher *watcher = new QDBusServiceWatcher(this);
    watcher->setConnection(QDBusConnection::sessionBus());
    watcher->addWatchedService("com.cute.Settings");
    connect(watcher, &QDBusServiceWatcher::serviceRegistered, this, &DesktopSettings::init);
    init();
}

QString DesktopSettings::wallpaper() const
{
    return m_wallpaper;//
}

bool DesktopSettings::backgroundVisible() const
{
    return m_interface.property("backgroundVisible").toBool();
}

bool DesktopSettings::dimsWallpaper() const
{
    return m_interface.property("darkModeDimsWallpaer").toBool();
}

int DesktopSettings::backgroundType() const
{
    return m_interface.property("backgroundType").toInt();
}

QString DesktopSettings::backgroundColor() const
{
    return m_interface.property("backgroundColor").toString();
}

void DesktopSettings::launch(const QString &command, const QStringList &args)
{
    QProcess process;
    process.setProgram(command);
    process.setArguments(args);
    process.startDetached();
}

void DesktopSettings::init()
{
    if (m_interface.isValid()) {
        connect(&m_interface, SIGNAL(wallpaperChanged(QString)), this, SLOT(onWallpaperChanged(QString)));
        connect(&m_interface, SIGNAL(darkModeDimsWallpaerChanged()), this, SIGNAL(dimsWallpaperChanged()));
        connect(&m_interface, SIGNAL(backgroundTypeChanged()), this, SIGNAL(backgroundTypeChanged()));
        connect(&m_interface, SIGNAL(backgroundColorChanged()), this, SIGNAL(backgroundColorChanged()));
        connect(&m_interface, SIGNAL(backgroundVisibleChanged()), this, SIGNAL(backgroundVisibleChanged()));

        m_wallpaper = m_interface.property("wallpaper").toString();
        emit wallpaperChanged();
    }
}

void DesktopSettings::onWallpaperChanged(QString path)
{
    if (path != m_wallpaper) {
        m_wallpaper = path;
        emit wallpaperChanged();
    }
}
