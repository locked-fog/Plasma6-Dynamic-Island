cmake_minimum_required(VERSION 3.24)
project(Plasma6DynamicIsland VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Qt6 requirement
find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick WaylandClient DBus)

# KDE Frameworks (optional but recommended)
find_package(KF6 CONFIG OPTIONAL_COMPONENTS
    ConfigCore
    ConfigGui
    DBusAddons
    Solid
    Notifications
    WindowSystem
)

# Check for Plasma components
if(NOT KF6ConfigCore_FOUND)
    message(WARNING "KDE Config framework not found, using basic settings")
endif()

# Main executable
add_executable(${PROJECT_NAME}
    main.cpp
    plasma/PlasmaBackend.cpp
    plasma/WindowManager.cpp
)

# Qt Resource file for QML
qt_add_resources(${PROJECT_NAME} "qml_qrc"
    PREFIX "/"
    FILES
    qml.qrc
    qml/main.qml
)

# Qt linking
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
    Qt6::WaylandClient
    Qt6::DBus
)

# KDE linking (if available)
if(KF6ConfigCore_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE KF6::ConfigCore KF6::ConfigGui)
endif()

if(KF6DBusAddons_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE KF6::DBusAddons)
endif()

if(KF6Solid_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE KF6::Solid)
endif()

if(KF6Notifications_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE KF6::Notifications)
endif()

if(KF6WindowSystem_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE KF6::WindowSystem)
endif()

# QML import path
set(QML_IMPORT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/qml CACHE PATH "QML import paths")
