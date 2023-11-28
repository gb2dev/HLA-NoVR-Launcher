import QtMultimedia
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Effects
import QtQuick.Window

Window {
    id: launcherWindow
    visible: true
    color: "black"
    maximumHeight: 800
    minimumHeight: 800
    maximumWidth: 1280
    minimumWidth: 1280
    title: qsTr("Half-Life: Alyx NoVR Launcher")

    onClosing: (close) => {
        if (!playButton.enabled) {
            close.accepted = false;
            errorLabel.text = "Installation in progress!\nPlease wait.";
            error.open();
        }
    }

    Component.onCompleted: function() {
        if (audioWarning) {
            errorLabel.text = "Warning: Only Stereo/Mono audio devices are supported by the game!";
            error.open();
        }
    }

    Connections {
        id: connections
        target: launcher
        function onUpdateModInstalling() {
            playButton.text = "Installing...";
        }
        function onUpdateModFinished() {
            playButton.text = "Update/Install NoVR";
            playButton.enabled = true;
            quitButton.enabled = true;
            error.close();
        }
        function onErrorMessage(message) {
            connections.onUpdateModFinished()
            errorLabel.text = message;
            error.open();
        }
    }

    FolderDialog {
        id: folderDialog
        currentFolder: "file:///C:/Program Files (x86)/Steam/steamapps/common/Half-Life Alyx"
        onAccepted: {
            launcher.updateMod(folderDialog.selectedFolder)
            if (launcher.validInstallation) {
                playButton.text = "Downloading...";
                playButton.enabled = false;
                quitButton.enabled = false;
            } else {
                errorLabel.text = "Half-Life: Alyx installation not found!\nPlease try again.";
                error.open();
            }
        }
    }

    Popup {
        id: error
        anchors.centerIn: parent
        height: 100
        width: 600
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        Label {
            id: errorLabel
            anchors.fill: parent
            font.pointSize: 24
            wrapMode: "WordWrap"
        }
    }

    Video {
        id: video
        anchors.fill: parent
        source: "file:///" + applicationDirPath + "/background.webm"
        fillMode: VideoOutput.PreserveAspectCrop
        loops: MediaPlayer.Infinite
        Component.onCompleted: play()
    }

    Column {
        id: column
        anchors.centerIn: parent
        spacing: 10

        Image {
            source: "file:///" + applicationDirPath + "/logo.png"
        }

        Item {
            height: 10
            width: parent.width
        }

        Button {
            id: playButton
            width: 217
            height: 34
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Play")
            enabled: launcher.validInstallation
            onClicked: launcher.playGame()
        }

        Button {
            id: quitButton
            width: 217
            height: 34
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Quit")
            onClicked: Qt.quit()
        }
    }

    Column {
        anchors.bottom: parent.bottom
        width: parent.width

        Label {
            id: launchOptionsTextFieldLabel
            color: "white"
            text: qsTr("Custom launch options:")
            font.bold: true
            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowBlur: 0.2
            }
        }

        TextField {
            id: launchOptionsTextField
            width: parent.width
            focus: true
            text: launcher.customLaunchOptions
            onTextChanged: launcher.customLaunchOptions = text
        }
    }
}
