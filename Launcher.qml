import QtCore
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
            errorLabel.text = qsTr("Installation in progress!\nPlease wait.");
            error.open();
        }
    }

    Component.onCompleted: {
        if (audioWarning) {
            errorLabel.text = qsTr("Warning: Only Stereo/Mono audio devices are supported by the game!");
            error.open();
        }
    }

    Connections {
        id: connections
        target: launcher
        function onUpdateModInstalling() {
            playButton.text = qsTr("Installing...");
        }
        function onErrorMessage(message) {
            playButton.text = "Play";
            playButton.enabled = true;
            quitButton.enabled = true;
            errorLabel.text = message;
            error.open();
        }
        function onInstallationSelectionNeeded() {
            playButton.text = qsTr("Play");
            folderDialog.open();
        }
    }

    FolderDialog {
        id: folderDialog
        onAccepted: {
            launcher.updateMod(folderDialog.selectedFolder)
            if (launcher.validInstallation) {
                playButton.text = "Downloading...";
            } else {
                playButton.enabled = true;
                quitButton.enabled = true;
                errorLabel.text = qsTr("Half-Life: Alyx installation not found!\nPlease try again.");
                error.open();
            }
        }
        onRejected: {
            playButton.enabled = true;
            quitButton.enabled = true;
        }
    }

    Popup {
        id: error
        anchors.centerIn: parent
        height: contentChildren.height
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
            width: 200
            height: 34
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Play")
            onClicked: {
                playButton.text = qsTr("Downloading...");
                playButton.enabled = false;
                quitButton.enabled = false;

                launcher.updateMod()
            }
        }

        Button {
            id: quitButton
            width: 200
            height: 34
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Quit")
            onClicked: Qt.quit()
        }
    }

    Column {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        Label {
            id: modBranchTextFieldLabel
            color: "white"
            text: qsTr("Mod branch:")
            font.bold: true
            layer.enabled: true
            layer.effect: MultiEffect {
                shadowEnabled: true
                shadowBlur: 0.2
            }
        }

        TextField {
            id:  modBranchTextField
            width: 400
            text: launcher.modBranch
            onTextChanged: launcher.modBranch = text
        }

        Item {
            height: 10
            width: parent.width
        }

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
            width: 400
            focus: true
            text: launcher.customLaunchOptions
            onTextChanged: launcher.customLaunchOptions = text
        }
    }
}
