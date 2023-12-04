import QtQuick
import QtQuick.Controls

Window {
    id: window
    visible: true
    color: "white"
    maximumHeight: 800
    minimumHeight: 800
    maximumWidth: 1280
    minimumWidth: 1280
    title: qsTr("Half-Life: Alyx NoVR Launcher")

    Label {
        color: "black"
        anchors.centerIn: parent
        font.pointSize: 24
        text: "Updating Launcher..."
    }
}
