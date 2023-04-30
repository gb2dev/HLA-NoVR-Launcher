import QtMultimedia
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Window

Window {
    id: window
    visible: true
    color: "#fff"
    maximumHeight: 408
    minimumHeight: 408
    maximumWidth: 976
    minimumWidth: 976
    title: qsTr("Half-Life: Alyx NoVR Launcher")

    onClosing: (close) => {
        if (!updateButton.enabled) {
            close.accepted = false;
        }
    }

    Label {
        id: errorLabel
        anchors.centerIn: parent
        font.pointSize: 24
        text: "Updating..."
    }
}
