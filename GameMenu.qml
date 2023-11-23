import QtQuick
import QtQuick.Controls
import QtQuick.Window

Window {
    id: gameMenuWindow
    flags: Qt.FramelessWindowHint
    color: "transparent"

    Component.onCompleted: gameMenu.gameStarted(this)

    Column {
        id: column
        anchors.centerIn: parent

        Button {
            id: buttonPlay
            text: qsTr("Continue")
            onClicked: gameMenu.buttonPlayClicked()
        }

        Button {
            id: buttonOptions
            text: qsTr("Options")
            onClicked: gameMenu.buttonOptionsClicked()
        }

        Button {
            id: buttonMainMenu
            visible: gameMenu.pauseMenuMode
            text: qsTr("Main Menu")
            onClicked: gameMenu.buttonMainMenuClicked()
        }

        Button {
            id: buttonQuit
            text: qsTr("Quit")
            onClicked: gameMenu.buttonQuitClicked()
        }
    }
}
