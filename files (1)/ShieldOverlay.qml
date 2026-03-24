// ============================================================
//  DeepfakeGuardian :: ShieldOverlay.qml
//  Transparent always-on-top overlay.
//  Color transitions: Green (safe) → Yellow (suspicious) → Red (threat)
// ============================================================
import QtQuick
import QtQuick.Controls.Material
import QtQuick.Effects

Window {
    id: root
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
    color: "transparent"
    opacity: 1.0
    width:  320
    height: 110

    // ---- C++ backend binding --------------------------------
    // ShieldViewModel exposes: trustScore (real), label (string),
    //                          privacyMode (bool)
    required property var viewModel

    // ---- Derived color from trust score --------------------
    readonly property color shieldColor: {
        if (viewModel.trustScore > 0.65) return "#22c55e"   // green-500
        if (viewModel.trustScore > 0.35) return "#f59e0b"   // amber-500
        return "#ef4444"                                      // red-500
    }

    // ---- Glowing border rectangle --------------------------
    Rectangle {
        id: frame
        anchors.fill: parent
        radius: 14
        color: "#0d0d0d"
        opacity: 0.88
        border.color: root.shieldColor
        border.width: 2

        Behavior on border.color {
            ColorAnimation { duration: 400; easing.type: Easing.InOutCubic }
        }

        // Ambient glow via MultiEffect
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled:  true
            shadowColor:    root.shieldColor
            shadowBlur:     0.6
            shadowHorizontalOffset: 0
            shadowVerticalOffset:   0
        }

        // ---- Shield icon -----------------------------------
        Text {
            id: icon
            anchors { left: parent.left; leftMargin: 14; verticalCenter: parent.verticalCenter }
            text: viewModel.privacyMode ? "🔴" : "🛡"
            font.pixelSize: 32
        }

        // ---- Trust score gauge (thin bar) ------------------
        Column {
            anchors {
                left: icon.right; leftMargin: 12
                right: killBtn.left; rightMargin: 10
                verticalCenter: parent.verticalCenter
            }
            spacing: 6

            Text {
                text: viewModel.label
                color: root.shieldColor
                font { pixelSize: 13; weight: Font.Bold; letterSpacing: 1.5 }
                Behavior on color { ColorAnimation { duration: 300 } }
            }

            // Animated trust bar
            Rectangle {
                width: parent.width; height: 6; radius: 3
                color: "#2a2a2a"
                Rectangle {
                    width: parent.width * viewModel.trustScore
                    height: 6; radius: 3
                    color: root.shieldColor
                    Behavior on width { NumberAnimation { duration: 350; easing.type: Easing.OutCubic } }
                    Behavior on color { ColorAnimation { duration: 350 } }
                }
            }

            Text {
                text: "Trust: " + Math.round(viewModel.trustScore * 100) + "%  |  Local Processing Only"
                color: "#6b7280"
                font.pixelSize: 10
            }
        }

        // ---- Kill-switch button ----------------------------
        Rectangle {
            id: killBtn
            anchors { right: parent.right; rightMargin: 14; verticalCenter: parent.verticalCenter }
            width: 36; height: 36; radius: 18
            color: killArea.containsMouse ? "#7f1d1d" : "#3f0a0a"
            border.color: "#ef4444"; border.width: 1

            Text { anchors.centerIn: parent; text: "✕"; color: "#ef4444"; font.pixelSize: 16 }

            MouseArea {
                id: killArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: viewModel.togglePrivacyMode()
            }
        }
    }

    // ---- Drag to reposition --------------------------------
    MouseArea {
        anchors.fill: parent
        property point clickPos
        onPressed: (mouse) => { clickPos = Qt.point(mouse.x, mouse.y) }
        onPositionChanged: (mouse) => {
            root.x += mouse.x - clickPos.x
            root.y += mouse.y - clickPos.y
        }
    }
}
