#include "PlasmaBackend.h"

#include <QCoreApplication>
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QRegularExpression>
#include <QDBusMessage>
#include <QDBusConnection>

PlasmaBackend* PlasmaBackend::s_instance = nullptr;

PlasmaBackend::PlasmaBackend(QObject *parent)
    : QObject(parent)
{
    s_instance = this;
}

PlasmaBackend* PlasmaBackend::instance()
{
    if (!s_instance) {
        s_instance = new PlasmaBackend();
    }
    return s_instance;
}

void PlasmaBackend::init()
{
    // Initial refresh
    refreshBattery();
    refreshVolume();
    refreshBrightness();

    // Setup polling for caps lock
    m_capsLockTimer.setInterval(500);
    QObject::connect(&m_capsLockTimer, &QTimer::timeout, this, &PlasmaBackend::setupCapsLockMonitoring);
    m_capsLockTimer.start();
}

void PlasmaBackend::refreshBattery()
{
    // Try reading from /sys/class/power_supply/
    QDir powerDir("/sys/class/power_supply");
    if (powerDir.exists()) {
        for (const QString& entry : powerDir.entryList()) {
            if (entry.startsWith("BAT")) {
                QFile capacityFile(powerDir.filePath(entry + "/capacity"));
                QFile statusFile(powerDir.filePath(entry + "/status"));

                if (capacityFile.open(QIODevice::ReadOnly)) {
                    m_batteryCapacity = QString::fromUtf8(capacityFile.readAll()).trimmed().toInt();
                    capacityFile.close();
                }

                if (statusFile.open(QIODevice::ReadOnly)) {
                    QString status = QString::fromUtf8(statusFile.readAll()).trimmed().toLower();
                    if (status == "charging") {
                        m_batteryStatus = 2;
                        m_isCharging = true;
                    } else if (status == "full") {
                        m_batteryStatus = 3;
                        m_isCharging = false;
                    } else {
                        m_batteryStatus = 1;
                        m_isCharging = false;
                    }
                    statusFile.close();
                }

                emit batteryCapacityChanged();
                emit chargingStateChanged();
                emit batteryStatusChanged();
                return;
            }
        }
    }

    // No battery found
    m_batteryCapacity = -1;
    m_batteryStatus = 0;
    emit batteryCapacityChanged();
    emit batteryStatusChanged();
}

void PlasmaBackend::refreshVolume()
{
    // Use wpctl (PipeWire) to get volume
    QProcess process;
    process.start("wpctl", {"get-volume", "@DEFAULT_AUDIO_SINK@"});
    process.waitForFinished(500);

    if (process.exitCode() == 0) {
        QString output = QString::fromUtf8(process.readAll());
        // Output format: "Volume: 0.50" or "Volume: 0.50 [MUTED]"
        if (output.contains("[MUTED]")) {
            m_isMuted = true;
        } else {
            m_isMuted = false;
            // Extract volume percentage
            QRegularExpression re("(\\d+\\.?\\d*)");
            QRegularExpressionMatch match = re.match(output);
            if (match.hasMatch()) {
                m_volume = static_cast<int>(match.captured(1).toDouble() * 100);
            }
        }
    }

    emit volumeChanged();
}

void PlasmaBackend::refreshBrightness()
{
    // Read from backlight sysfs
    QDir backlightDir("/sys/class/backlight");
    if (backlightDir.exists()) {
        for (const QString& entry : backlightDir.entryList()) {
            QFile brightnessFile(backlightDir.filePath(entry + "/brightness"));
            QFile maxBrightnessFile(backlightDir.filePath(entry + "/max_brightness"));

            int maxBrightness = 100;
            int currentBrightness = 50;

            if (maxBrightnessFile.open(QIODevice::ReadOnly)) {
                maxBrightness = QString::fromUtf8(maxBrightnessFile.readAll()).trimmed().toInt();
                maxBrightnessFile.close();
            }

            if (brightnessFile.open(QIODevice::ReadOnly)) {
                currentBrightness = QString::fromUtf8(brightnessFile.readAll()).trimmed().toInt();
                brightnessFile.close();
            }

            if (maxBrightness > 0) {
                m_brightness = static_cast<int>((currentBrightness * 100.0) / maxBrightness);
            }

            emit brightnessChanged();
            return;
        }
    }

    // No backlight found, emit anyway
    emit brightnessChanged();
}

void PlasmaBackend::setVolume(int value)
{
    m_volume = qBound(0, value, 100);
    QProcess process;
    process.start("wpctl", {"set-volume", "@DEFAULT_AUDIO_SINK@", QString::number(m_volume / 100.0)});
    process.waitForFinished(500);
    emit volumeChanged();
}

void PlasmaBackend::setBrightness(int value)
{
    m_brightness = qBound(1, value, 100);

    // Write to backlight sysfs
    QDir backlightDir("/sys/class/backlight");
    if (backlightDir.exists()) {
        for (const QString& entry : backlightDir.entryList()) {
            QFile maxBrightnessFile(backlightDir.filePath(entry + "/max_brightness"));
            int maxBrightness = 100;

            if (maxBrightnessFile.open(QIODevice::ReadOnly)) {
                maxBrightness = QString::fromUtf8(maxBrightnessFile.readAll()).trimmed().toInt();
                maxBrightnessFile.close();
            }

            int sysBrightness = static_cast<int>((m_brightness / 100.0) * maxBrightness);
            QFile brightnessFile(backlightDir.filePath(entry + "/brightness"));
            if (brightnessFile.open(QIODevice::WriteOnly)) {
                brightnessFile.write(QString::number(sysBrightness).toUtf8());
                brightnessFile.close();
            }

            emit brightnessChanged();
            return;
        }
    }
}

void PlasmaBackend::toggleMute()
{
    QProcess process;
    process.start("wpctl", {"set-mute", "@DEFAULT_AUDIO_SINK@", "toggle"});
    process.waitForFinished(500);
    m_isMuted = !m_isMuted;
    emit volumeChanged();
}

void PlasmaBackend::sendNotification(const QString& title, const QString& body)
{
    // Use KDE notifications via DBus
    QDBusMessage msg = QDBusMessage::createMethodCall(
        "org.freedesktop.Notifications",
        "/org/freedesktop/Notifications",
        "org.freedesktop.Notifications",
        "Notify"
    );

    QList<QVariant> args;
    args << QVariant("Plasma6DynamicIsland")  // app_name
         << QVariant(0u)                       // replaces_id
         << QVariant(QString())                 // app_icon
         << QVariant(title)                     // summary
         << QVariant(body)                      // body
         << QVariantList()                      // actions
         << QVariantMap();                      // hints
    msg.setArguments(args);

    QDBusConnection::sessionBus().send(msg);
}

void PlasmaBackend::setupCapsLockMonitoring()
{
    QProcess process;
    process.start("xset", {"q"});
    process.waitForFinished(100);

    if (process.exitCode() == 0) {
        QString output = QString::fromUtf8(process.readAll());
        bool newState = output.contains(QRegularExpression("Caps Lock:\\s+on", QRegularExpression::CaseInsensitiveOption));
        if (newState != m_capsLockActive) {
            m_capsLockActive = newState;
            emit capsLockChanged();
        }
    }
}

void PlasmaBackend::setupBluetoothMonitoring()
{
    // Monitor bluetooth via dbus
}
