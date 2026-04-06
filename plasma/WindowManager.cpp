#include "WindowManager.h"

#include <QGuiApplication>
#include <QScreen>
#include <QCursor>

WindowManager* WindowManager::s_instance = nullptr;

WindowManager::WindowManager(QObject *parent)
    : QObject(parent)
{
    s_instance = this;
}

WindowManager* WindowManager::instance()
{
    if (!s_instance) {
        s_instance = new WindowManager();
    }
    return s_instance;
}

void WindowManager::init()
{
    // Get primary screen dimensions
    if (QGuiApplication::primaryScreen()) {
        QSize size = QGuiApplication::primaryScreen()->size();
        m_screenWidth = size.width();
        m_screenHeight = size.height();
    }
}

void WindowManager::setSidebarWindow(QQuickWindow* window)
{
    m_sidebarWindow = window;
    if (window) {
        positionSidebar();
    }
    emit sidebarWindowChanged();
}

void WindowManager::positionSidebar()
{
    if (!m_sidebarWindow) return;

    // Position on the right side of the screen
    int sidebarWidth = 380;
    m_sidebarWindow->setGeometry(
        m_screenWidth - sidebarWidth,  // x (right edge)
        0,                              // y (top)
        sidebarWidth,                   // width
        m_screenHeight                  // height (full height)
    );

    // Set window flags for always on top, frameless, etc.
    m_sidebarWindow->setFlags(
        Qt::Window |
        Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint |
        Qt::WindowDoesNotAcceptFocus |
        Qt::BypassWindowManagerHint
    );

    m_sidebarWindow->setVisible(true);
    m_sidebarWindow->raise();
}

void WindowManager::createIslandWindow(const QString& islandType, int x, int y, int width, int height)
{
    // This would be called from QML to create floating islands
    // The actual window creation is handled via QML Loader in the main application
    QString islandId = islandType + "_" + QString::number(QDateTime::currentMSecsSinceEpoch());
    emit islandWindowCreated(islandId, islandType);
}

void WindowManager::closeIslandWindow(const QString& islandId)
{
    // Find and close the island window
    for (auto it = m_islands.begin(); it != m_islands.end(); ++it) {
        if (it->id == islandId) {
            if (it->window) {
                it->window->close();
            }
            m_islands.erase(it);
            emit islandWindowClosed(islandId);
            return;
        }
    }
}

QPoint WindowManager::getMousePosition()
{
    return QCursor::pos();
}
