#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QSurfaceFormat>

#include "plasma/PlasmaBackend.h"
#include "plasma/WindowManager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("Plasma6DynamicIsland");
    app.setOrganizationName("Archipelago");

    QQmlApplicationEngine engine;

    // Register C++ types
    qmlRegisterSingletonType<PlasmaBackend>("Archipelago", 1, 0, "Backend",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return PlasmaBackend::instance();
        });

    qmlRegisterSingletonType<WindowManager>("Archipelago", 1, 0, "WindowManager",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return WindowManager::instance();
        });

    // Add import path
    engine.addImportPath("qrc:/");
    engine.addImportPath(":/");

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(EXIT_FAILURE);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
