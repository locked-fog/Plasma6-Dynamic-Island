import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Effects

import Archipelago 1.0

Window {
    id: sidebarWindow
    width: 160  // Collapsed sidebar width
    height: sidebarLoader.height + 20
    x: Screen.width - width - 10
    y: 100
    visible: true
    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.BypassWindowManagerHint | Qt.WindowDoesNotAcceptFocus
    color: "transparent"

    // Floating island aesthetic with blur background
    background: Rectangle {
        color: Qt.rgba(0.08, 0.09, 0.10, 0.85)
        radius: 34
        border.width: 1
        border.color: Qt.rgba(1, 1, 1, 0.08)
    }

    // Main content loader - handles morphing
    Item {
        id: sidebarLoader
        anchors.centerIn: parent
        width: mainIsland.width
        height: mainIsland.height

        // The morphing island capsule
        Rectangle {
            id: mainIsland
            width: islandStateHandler.targetWidth
            height: islandStateHandler.targetHeight
            radius: islandStateHandler.targetRadius
            color: islandStateHandler.currentColor
            clip: true

            // Inner glow/border
            Rectangle {
                anchors.fill: parent
                anchors.margins: 1
                radius: parent.radius - 1
                color: "transparent"
                border.width: 1
                border.color: Qt.rgba(1, 1, 1, 0.06)
            }

            // Morphing animations
            Behavior on width { NumberAnimation { duration: 400; easing.type: Easing.OutQuint } }
            Behavior on height { NumberAnimation { duration: 400; easing.type: Easing.OutQuint } }
            Behavior on radius { NumberAnimation { duration: 400; easing.type: Easing.OutQuint } }
            Behavior on color { ColorAnimation { duration: 280; easing.type: Easing.InOutQuad } }

            // --- State Handler ---
            QtObject {
                id: islandStateHandler

                // Current state
                property string currentState: "compact"  // compact, expanded, notification, media

                // Target dimensions based on state
                readonly property real targetWidth: {
                    switch (currentState) {
                        case "notification": return 280
                        case "media": return 320
                        case "expanded": return 360
                        default: return 140
                    }
                }
                readonly property real targetHeight: {
                    switch (currentState) {
                        case "notification": return 64
                        case "media": return 80
                        case "expanded": return 200
                        default: return 68
                    }
                }
                readonly property real targetRadius: {
                    switch (currentState) {
                        case "notification": return 32
                        case "media": return 40
                        case "expanded": return 40
                        default: return 34
                    }
                }
                readonly property color currentColor: "#0a0a0a"
            }

            // Content layers
            Column {
                anchors.centerIn: parent
                spacing: 0

                // Clock/Current time display
                Row {
                    id: clockRow
                    spacing: 8
                    opacity: islandStateHandler.currentState === "compact" ? 1 : 0

                    Text {
                        id: timeText
                        text: new Date().toLocaleTimeString(Qt.locale("en_US"), "HH:mm")
                        color: "#FFFFFF"
                        font.pixelSize: 24
                        font.weight: Font.Medium
                        font.family: "Inter"
                    }
                }

                // Notification content
                Item {
                    id: notificationContent
                    width: 240
                    height: 48
                    anchors.centerIn: parent
                    visible: islandStateHandler.currentState === "notification"
                    opacity: visible ? 1 : 0

                    Row {
                        anchors.centerIn: parent
                        spacing: 12

                        Text {
                            text: "🔔"
                            color: "#FFFFFF"
                            font.pixelSize: 20
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Column {
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 2

                            Text {
                                text: "Notification"
                                color: "#FFFFFF"
                                font.pixelSize: 14
                                font.weight: Font.Medium
                            }
                            Text {
                                text: "Notification body text"
                                color: "#888888"
                                font.pixelSize: 11
                            }
                        }
                    }
                }

                // Media content
                Item {
                    id: mediaContent
                    width: 280
                    height: 64
                    anchors.centerIn: parent
                    visible: islandStateHandler.currentState === "media"
                    opacity: visible ? 1 : 0

                    Row {
                        anchors.centerIn: parent
                        spacing: 12

                        Rectangle {
                            width: 48
                            height: 48
                            radius: 8
                            color: Qt.rgba(1, 1, 1, 0.1)
                            anchors.verticalCenter: parent.verticalCenter

                            Text {
                                text: "🎵"
                                color: "#FFFFFF"
                                font.pixelSize: 20
                                anchors.centerIn: parent
                            }
                        }

                        Column {
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 2

                            Text {
                                text: "Track Title"
                                color: "#FFFFFF"
                                font.pixelSize: 14
                                font.weight: Font.Medium
                            }
                            Text {
                                text: "Artist Name"
                                color: "#888888"
                                font.pixelSize: 12
                            }
                        }
                    }
                }
            }

            // Click handler
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    // Toggle through states for demo
                    var states = ["compact", "notification", "media", "expanded"]
                    var currentIndex = states.indexOf(islandStateHandler.currentState)
                    islandStateHandler.currentState = states[(currentIndex + 1) % states.length]
                }
            }
        }
    }

    // Sidebar expansion handle
    Rectangle {
        id: expandHandle
        width: 8
        height: 60
        radius: 4
        color: Qt.rgba(1, 1, 1, 0.15)
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: -4

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: parent.color = Qt.rgba(1, 1, 1, 0.3)
            onExited: parent.color = Qt.rgba(1, 1, 1, 0.15)
        }
    }

    // Update time
    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            timeText.text = new Date().toLocaleTimeString(Qt.locale("en_US"), "HH:mm")
        }
    }

    Component.onCompleted: {
        WindowManager.setSidebarWindow(sidebarWindow)
        Backend.init()
    }
}
