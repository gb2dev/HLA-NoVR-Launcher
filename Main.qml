import QtMultimedia
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Window

Window {
    id: window
    visible: true
    color: "#000000"
    maximumHeight: 408
    minimumHeight: 408
    maximumWidth: 976
    minimumWidth: 976
    title: qsTr("Half-Life: Alyx NoVR Launcher")


    Connections {
        target: launcher
        function onUpdateModInstalling() {
            updateButton.text = "Installing...";
        }
        function onUpdateModFinished() {
            updateButton.text = "Update/Install";
            updateButton.enabled = true;
        }
    }

    FolderDialog {
        id: folderDialog
        currentFolder: "file:///C:/Program Files (x86)/Steam/steamapps/common/Half-Life Alyx"
        onAccepted: {
            updateButton.enabled = false;
            updateButton.text = "Downloading...";
            launcher.updateMod(folderDialog.selectedFolder)
        }
    }

    Video {
        id: video
        anchors.fill: parent
        source: "file:/output.webm"
        fillMode: VideoOutput.PreserveAspectCrop
        loops: MediaPlayer.Infinite
        Component.onCompleted: play()
    }

    Column {
        id: column
        width: 228
        height: 400
        anchors.left: parent.left
        anchors.top: parent.top
        spacing: 2
        anchors.leftMargin: 34
        anchors.topMargin: 188

        Button {
            id: playButton
            width: 217
            height: 34
            text: "Play"
            onClicked: {
                launcher.playGame()
                Qt.quit()
            }
        }

        Button {
            id: updateButton
            width: 217
            height: 34
            text: "Update/Install"
            onClicked: folderDialog.open()
        }

        Button {
            id: optionsButton
            width: 217
            height: 34
            text: "Options"
            enabled: false
        }

        Button {
            id: quitButton
            width: 217
            height: 34
            text: "Quit"
            onClicked: Qt.quit()
        }
    }

    Label {
        id: label
        y: 463
        width: 262
        height: 15
        color: "#ffffff"
        text: "NoVR by GB_2 Development Team"
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        font.bold: true
        anchors.bottomMargin: 2
        anchors.leftMargin: 2
        font.family: "Tahoma"
    }
}
