#pragma once

#include <QObject>
#include <QQuickWindow>
#include <QPoint>
#include <QQmlEngine>

class WindowManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickWindow* sidebarWindow READ sidebarWindow CONSTANT)
    Q_PROPERTY(int screenWidth READ screenWidth CONSTANT)
    Q_PROPERTY(int screenHeight READ screenHeight CONSTANT)

public:
    static WindowManager* instance();

    QQuickWindow* sidebarWindow() const { return m_sidebarWindow; }
    int screenWidth() const { return m_screenWidth; }
    int screenHeight() const { return m_screenHeight; }

    Q_INVOKABLE void setSidebarWindow(QQuickWindow* window);
    Q_INVOKABLE void positionSidebar();
    Q_INVOKABLE void createIslandWindow(const QString& islandType, int x, int y, int width, int height);
    Q_INVOKABLE void closeIslandWindow(const QString& islandId);
    Q_INVOKABLE QPoint getMousePosition();

public slots:
    void init();

signals:
    void sidebarWindowChanged();
    void islandWindowCreated(const QString& islandId, const QString& islandType);
    void islandWindowClosed(const QString& islandId);

private:
    explicit WindowManager(QObject *parent = nullptr);
    ~WindowManager() = default;

    static WindowManager* s_instance;

    QQuickWindow* m_sidebarWindow = nullptr;
    int m_screenWidth = 1920;
    int m_screenHeight = 1080;

    struct IslandInfo {
        QString id;
        QString type;
        QQuickWindow* window;
    };
    QList<IslandInfo> m_islands;
};
