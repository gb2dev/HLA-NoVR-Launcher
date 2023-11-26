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
                    savesListView.visible = false;
                    chaptersListView.visible = false;
                    addonsListView.visible = false;
                    break;
                case 1: // HUD
                    menu.visible = false;
                    //labelHudHealth.visible = true;
                    savesListView.visible = false;
                    chaptersListView.visible = false;
                    break;
                case 2: // PauseMenu
                    menu.visible = true;
                    buttonPlay.visible = true;
                    buttonLoadGame.visible = true;
                    buttonMainMenu.visible = true;
                    buttonAddons.visible = false;
                    savesListView.visible = false;
                    chaptersListView.visible = false;
                    savesModel.clear();
                    savesModel.append({ saveName: "Cancel", saveFileName: "cancel" });
                    break;
                case 3: // MainMenu
                    if (chaptersListView.visible) {
                        chaptersListView.visible = false;
                        menu.visible = true;
                    } else if (savesListView.visible) {
                        savesListView.visible = false;
                        menu.visible = true;
                    } else if (addonsListView.visible) {
                        addonsListView.visible = false;
                        menu.visible = true;
                    } else {
                        mainMenuTimer.start();
                    }
                    buttonAddons.visible = true;
                    savesModel.clear();
                    savesModel.append({ saveName: "Cancel", saveTimeDate: "", saveFileName: "cancel" });
                    onAddonToggled();
                    addonMapsModel.clear();
                    addonMapsModel.append({ chapterName: "Cancel", chapterMapName: "cancel", mapEnabled: true });
                    addonMapsModel.append({ chapterName: "Original Game", chapterMapName: "original_game", mapEnabled: true });
                    break;
            }
        }
        function onSaveAdded(name, timeDate, fileName) {
            savesModel.append({ saveName: name, saveTimeDate: timeDate, saveFileName: fileName })
        }
        function onNewGameSelected() {
            chaptersListView.visible = true;
            menu.visible = false;
        }
        function onNoSaveFilesDetected() {
            buttonPlay.visible = false;
            buttonLoadGame.visible = false;
        }
        function onAddonAdded(name, fileName) {
            addonsModel.append({ addonName: name, addonFileName: fileName })
        }
        function onAddonMapsAdded(maps, enabled) {
            maps.forEach(function(map) {
                if (map !== "") {
                    addonMapsModel.append({ chapterName: map, chapterMapName: map, addonMap: true, mapEnabled: enabled });
                }
            });
        }
        function onAddonToggled() {
            addonsModel.clear();
            addonsModel.append({ addonName: "Cancel", addonFileName: "cancel" });
            addonsModel.append({ addonName: "Workshop", addonFileName: "workshop" });
        }
    }

    ListModel {
        id: savesModel
    }

    ListView {
        id: savesListView
        anchors.top: menu.top
        anchors.bottom: parent.bottom
        width: 200
        anchors.horizontalCenter: parent.horizontalCenter
        model: savesModel
        delegate: Button {
            width: 200
            contentItem: Column {
                Label {
                    font.bold: saveFileName !== "cancel"
                    text: saveName
                    verticalAlignment: saveFileName === "cancel" ? Text.AlignHCenter : Text.AlignLeft
                }
                Label {
                    visible: saveFileName !== "cancel"
                    text: saveTimeDate
                    verticalAlignment: Text.AlignLeft
                }
            }
            onClicked: gameMenu.loadSave(saveFileName)
        }
    }

    ListModel {
        id: chaptersModel

        ListElement { chapterName: "Cancel"; chapterMapName: "cancel"; addonMap: false; mapEnabled: true; }
        ListElement { chapterName: "Addon Maps"; chapterMapName: "addon_maps"; addonMap: false; mapEnabled: true; }
        ListElement { chapterName: "Entanglement"; chapterMapName: "a1_intro_world"; addonMap: false; mapEnabled: true; }
        ListElement { chapterName: "The Quarantine Zone"; chapterMapName: "a2_quarantine_entrance"; addonMap: false; mapEnabled: true; }
        ListElement { chapterName: "Is or Will Be"; chapterMapName: "a2_headcrabs_tunnel"; addonMap: false; mapEnabled: true; }
        ListElement { chapterName: "Superweapon"; chapterMapName: "a3_station_street"; addonMap: false; mapEnabled: true; }
        ListElement { chapterName: "The Northern Star"; chapterMapName: "a3_hotel_lobby_basement"; addonMap: false; mapEnabled: true; }
        ListElement { chapterName: "Arms Race"; chapterMapName: "a3_c17_processing_plant"; addonMap: false; mapEnabled: true; }
        ListElement { chapterName: "Jeff"; chapterMapName: "a3_distillery"; addonMap: false; mapEnabled: true; }
        ListElement { chapterName: "Captivity"; chapterMapName: "a4_c17_zoo"; addonMap: false; mapEnabled: true; }
        ListElement { chapterName: "Revelations"; chapterMapName: "a4_c17_tanker_yard"; addonMap: false; mapEnabled: true; }
        ListElement { chapterName: "Breaking and Entering"; chapterMapName: "a4_c17_water_tower"; addonMap: false; mapEnabled: true; }
        ListElement { chapterName: "Point Extraction"; chapterMapName: "a5_vault"; addonMap: false; mapEnabled: true; }
    }

    ListModel {
        id: addonMapsModel
    }

    ListView {
        id: chaptersListView
        visible: false
        anchors.top: menu.top
        anchors.bottom: parent.bottom
        width: 200
        anchors.horizontalCenter: parent.horizontalCenter
        model: chaptersModel
        delegate: Button {
            enabled: mapEnabled
            width: 200
            text: chapterName
            onClicked: function() {
                if (chapterMapName === "addon_maps") {
                    chaptersListView.model = addonMapsModel
                } else if (chapterMapName === "original_game") {
                    chaptersListView.model = chaptersModel
                } else {
                    gameMenu.newGame(chapterMapName, addonMap);
                }
            }
        }
    }

    ListModel {
        id: addonsModel
    }

    ListView {
        id: addonsListView
        visible: false
        anchors.top: menu.top
        anchors.bottom: parent.bottom
        width: 200
        anchors.horizontalCenter: parent.horizontalCenter
        model: addonsModel
        delegate: Button {
            width: 200
            text: addonName
            onClicked: gameMenu.toggleAddon(addonFileName)
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

        Button { // TODO: Hide if no save file yet
            id: buttonPlay
            width: 50
            text: qsTr("Continue")
            Layout.fillWidth: true
            onClicked: gameMenu.buttonPlayClicked();
        }

        Button {
            id: buttonLoadGame
            width: 50
            text: qsTr("Load Game")
            Layout.fillWidth: true
            onClicked: function() {
                menu.visible = false;
                savesListView.visible = true;
                gameMenu.buttonLoadGameClicked();
            }
        }

        Button {
            id: buttonNewGmae
            width: 50
            text: qsTr("New Game")
            Layout.fillWidth: true
            onClicked: gameMenu.buttonNewGameClicked();
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
            id: buttonAddons
            width: 50
            text: qsTr("Addons")
            Layout.fillWidth: true
            onClicked: function() {
                addonsListView.visible = true;
                menu.visible = false;
                gameMenu.buttonAddonsClicked();
            }
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
