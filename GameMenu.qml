import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts

Window {
    id: gameMenuWindow
    flags: Qt.FramelessWindowHint
    color: "transparent"

    Component.onCompleted: gameMenu.gameStarted(this)

    ColumnLayout {
        id: columnLayout
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
            visible: gameMenu.pauseMenuMode
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
}
