import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts

Window {
    id: gameMenuWindow
    visible: true
    flags: Qt.FramelessWindowHint | Qt.WindowDoesNotAcceptFocus
    color: "transparent"

    Component.onCompleted: gameMenu.gameStarted(this)

    Connections {
        id: connections
        target: gameMenu
        function onHudHealthChanged(hudHealth) {
            labelHudHealth.text = hudHealth;
        }
        function onVisibilityStateChanged(state) {
            switch (state) {
                case 0: // Hidden
                    menu.visible = false;
                    labelHudHealth.visible = false;
                    break;
                case 1: // HUD
                    menu.visible = false;
                    labelHudHealth.visible = true;
                    break;
                case 2: // PauseMenu
                    menu.visible = true;
                    buttonMainMenu.visible = true;
                    break;
                case 3: // MainMenu
                    mainMenuTimer.start();
                    break;
            }
        }
    }

    Timer {
        id: mainMenuTimer
        interval: 5000
        running: false
        repeat: false
        onTriggered: function() {
            menu.visible = true;
            buttonMainMenu.visible = false;
            labelHudHealth.visible = false;
        }
    }

    ColumnLayout {
        id: menu
        visible: false
        width: 100
        height: 100
        anchors.centerIn: parent

        Button {
            id: buttonPlay
            width: 50
            text: qsTr("Continue")
            Layout.fillWidth: true
            onClicked: gameMenu.buttonPlayClicked()
        }

        Button {
            id: buttonOptions
            width: 50
            text: qsTr("Options")
            Layout.fillWidth: true
            onClicked: gameMenu.buttonOptionsClicked()
        }

        Button {
            id: buttonMainMenu
            width: 50
            text: qsTr("Main Menu")
            Layout.fillWidth: true
            onClicked: gameMenu.buttonMainMenuClicked()
        }

        Button {
            id: buttonQuit
            width: 50
            text: qsTr("Quit")
            Layout.fillWidth: true
            onClicked: gameMenu.buttonQuitClicked()
        }
    }

    Label {
        id: labelHudHealth
        visible: false
        text: "100"
        color: "red"
        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }
}
