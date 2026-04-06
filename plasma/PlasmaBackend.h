#pragma once

#include <QObject>
#include <QVariant>
#include <QString>
#include <QTimer>

#ifdef Signals
#undef Signals
#endif

class PlasmaBackend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int batteryCapacity READ batteryCapacity NOTIFY batteryCapacityChanged)
    Q_PROPERTY(bool isCharging READ isCharging NOTIFY chargingStateChanged)
    Q_PROPERTY(int batteryStatus READ batteryStatus NOTIFY batteryStatusChanged)
    Q_PROPERTY(int volume READ volume NOTIFY volumeChanged)
    Q_PROPERTY(bool isMuted READ isMuted NOTIFY volumeChanged)
    Q_PROPERTY(int brightness READ brightness NOTIFY brightnessChanged)
    Q_PROPERTY(bool capsLockActive READ capsLockActive NOTIFY capsLockChanged)
    Q_PROPERTY(bool bluetoothActive READ bluetoothActive NOTIFY bluetoothChanged)

public:
    static PlasmaBackend* instance();

    // Battery
    int batteryCapacity() const { return m_batteryCapacity; }
    bool isCharging() const { return m_isCharging; }
    int batteryStatus() const { return m_batteryStatus; } // 0=Discharging, 1=Charging, 2=Full

    // Volume
    int volume() const { return m_volume; }
    bool isMuted() const { return m_isMuted; }

    // Brightness
    int brightness() const { return m_brightness; }

    // Keyboard
    bool capsLockActive() const { return m_capsLockActive; }

    // Bluetooth
    bool bluetoothActive() const { return m_bluetoothActive; }

    Q_INVOKABLE void setVolume(int value);
    Q_INVOKABLE void setBrightness(int value);
    Q_INVOKABLE void toggleMute();
    Q_INVOKABLE void sendNotification(const QString& title, const QString& body);

public slots:
    void init();
    void refreshBattery();
    void refreshVolume();
    void refreshBrightness();

signals:
    void batteryCapacityChanged();
    void chargingStateChanged();
    void batteryStatusChanged();
    void volumeChanged();
    void brightnessChanged();
    void capsLockChanged();
    void bluetoothChanged();

private:
    explicit PlasmaBackend(QObject *parent = nullptr);
    ~PlasmaBackend() = default;

    void setupSolidBackend();
    void setupDBusNotifications();
    void setupVolumeMonitoring();
    void setupCapsLockMonitoring();
    void setupBluetoothMonitoring();

    static PlasmaBackend* s_instance;

    // Battery state
    int m_batteryCapacity = 0;
    bool m_isCharging = false;
    int m_batteryStatus = 0; // 0=Unknown, 1=Discharging, 2=Charging, 3=Full

    // Volume state
    int m_volume = 50;
    bool m_isMuted = false;

    // Brightness state
    int m_brightness = 100;

    // Keyboard state
    bool m_capsLockActive = false;

    // Bluetooth state
    bool m_bluetoothActive = false;

    // Timers for polling
    QTimer m_capsLockTimer;
};
