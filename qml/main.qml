import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

import Archipelago 1.0

Window {
    id: sidebarWindow
    width: 380
    height: Screen.height
    x: Screen.width - width
    y: 0
    visible: true
    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.BypassWindowManagerHint
    color: "transparent"

    // Enable transparency
    Rectangle {
        id: sidebarBackground
        anchors.fill: parent
        color: Qt.rgba(0.15, 0.16, 0.13, 0.88)
        radius: 0
    }

    // Main sidebar content
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 4

        // Top section: Current time and status
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: "transparent"

            Row {
                anchors.centerIn: parent
                spacing: 12

                Text {
                    id: timeText
                    text: new Date().toLocaleTimeString(Qt.locale("en_US"), "HH:mm")
                    color: "#FFFFFF"
                    font.pixelSize: 32
                    font.weight: Font.Medium
                }

                Text {
                    id: dateText
                    text: new Date().toLocaleDateString(Qt.locale("en_US"), "ddd dd")
                    color: "#AAAAAA"
                    font.pixelSize: 14
                    anchors.bottom: timeText.bottom
                    anchors.bottomMargin: 6
                }
            }
        }

        // Divider
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Qt.rgba(1, 1, 1, 0.25)
        }

        // System Monitor Section
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            color: "transparent"

            Column {
                anchors.centerIn: parent
                spacing: 8

                // Battery
                Row {
                    spacing: 8
                    Text {
                        text: Backend.isCharging ? "⚡" : "🔋"
                        color: "#FFFFFF"
                        font.pixelSize: 16
                    }
                    Text {
                        text: Backend.batteryCapacity >= 0 ? Backend.batteryCapacity + "%" : "N/A"
                        color: "#FFFFFF"
                        font.pixelSize: 14
                    }
                }

                // Volume
                Row {
                    spacing: 8
                    Text {
                        text: Backend.isMuted ? "🔇" : "🔊"
                        color: "#FFFFFF"
                        font.pixelSize: 16
                    }
                    Text {
                        text: Backend.volume + "%"
                        color: "#FFFFFF"
                        font.pixelSize: 14
                    }
                }

                // Brightness
                Row {
                    spacing: 8
                    Text {
                        text: "☀️"
                        color: "#FFFFFF"
                        font.pixelSize: 16
                    }
                    Text {
                        text: Backend.brightness + "%"
                        color: "#FFFFFF"
                        font.pixelSize: 14
                    }
                }
            }
        }

        // Divider
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: Qt.rgba(1, 1, 1, 0.25)
        }

        // Quick actions placeholder
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            color: "transparent"

            GridLayout {
                anchors.fill: parent
                anchors.margins: 8
                columns: 3
                rowSpacing: 8
                columnSpacing: 8

                Repeater {
                    model: [
                        { icon: "🔊", label: "Volume" },
                        { icon: "☀️", label: "Brightness" },
                        { icon: "📱", label: "Devices" },
                        { icon: "📁", label: "Files" },
                        { icon: "⚙️", label: "Settings" },
                        { icon: "📋", label: "Clipboard" }
                    ]

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: Qt.rgba(1, 1, 1, 0.19)
                        radius: 12

                        Column {
                            anchors.centerIn: parent
                            spacing: 4

                            Text {
                                text: modelData.icon
                                color: "#FFFFFF"
                                font.pixelSize: 24
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                            Text {
                                text: modelData.label
                                color: "#AAAAAA"
                                font.pixelSize: 10
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                console.log("Clicked:", modelData.label)
                            }
                        }
                    }
                }
            }
        }

        // Drop zone placeholder for "中转站"
        Rectangle {
            id: dropZone
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 200
            color: dropArea.containsDrag ? Qt.rgba(1, 1, 1, 0.31) : Qt.rgba(1, 1, 1, 0.13)
            radius: 16
            border.width: 2
            border.color: dropArea.containsDrag ? Qt.rgba(1, 1, 1, 0.38) : Qt.rgba(1, 1, 1, 0.19)

            Column {
                anchors.centerIn: parent
                spacing: 8

                Text {
                    text: "📎"
                    color: "#FFFFFF"
                    font.pixelSize: 36
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Text {
                    text: "Drop here"
                    color: "#AAAAAA"
                    font.pixelSize: 14
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Text {
                    text: "拖放内容到此处暂存"
                    color: "#666666"
                    font.pixelSize: 12
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }

            DropArea {
                id: dropArea
                anchors.fill: parent
                keys: ["text/plain", "text/uri-list", "text/x-moz-url"]

                onEntered: (drag) => {
                    console.log("Drag entered with keys:", drag.keys)
                }

                onDropped: (drop) => {
                    console.log("Dropped:", drop.getDataAsString("text/plain"))
                    Backend.sendNotification("Content Staged", "Data has been staged for transfer")
                }
            }
        }
    }

    // Update time every second
    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            timeText.text = new Date().toLocaleTimeString(Qt.locale("en_US"), "HH:mm")
            dateText.text = new Date().toLocaleDateString(Qt.locale("en_US"), "ddd dd")
        }
    }

    Component.onCompleted: {
        WindowManager.setSidebarWindow(sidebarWindow)
        Backend.init()
    }
}
