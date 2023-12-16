import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts

Window {
    id: gameMenuWindow
    visible: false
    flags: Qt.FramelessWindowHint
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
                    options.visible = false;
                    labelHudHealth.visible = false;
                    savesListView.visible = false;
                    chaptersListView.visible = false;
                    addonsListView.visible = false;
                    break;
                case 1: // HUD
                    menu.visible = false;
                    options.visible = false;
                    //labelHudHealth.visible = true;
                    savesListView.visible = false;
                    chaptersListView.visible = false;
                    break;
                case 2: // PauseMenu
                    menu.visible = true;
                    buttonPlay.visible = true;
                    buttonSaveGame.visible = true;
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
                    options.visible = false;
                    buttonAddons.visible = true;
                    savesModel.clear();
                    savesModel.append({ saveName: "Cancel", saveDateTime: "", saveFileName: "cancel" });
                    onAddonToggled();
                    addonMapsModel.clear();
                    addonMapsModel.append({ chapterName: "Cancel", chapterMapName: "cancel", mapEnabled: true });
                    addonMapsModel.append({ chapterName: "Original Game", chapterMapName: "original_game", mapEnabled: true });
                    break;
            }
        }
        function onSaveAdded(name, dateTime, fileName) {
            savesModel.append({ saveName: name, saveDateTime: dateTime, saveFileName: fileName })
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
        function onBindingChanged(inputName, bind) {
            switch (inputName) {
                case "MOUSE_SENSITIVITY":
                    mouseSensitivity.value = bind;
                    break;
                case "INVERT_MOUSE_Y":
                    mouseInvertY.checked = bind === "true";
                    break;
                case "PRIMARY_ATTACK":
                    buttonPrimaryAttack.text = bind;
                    buttonPrimaryAttack.enabled = true;
                    break;
                case "SECONDATY_ATTACK":
                    buttonSecondaryAttack.text = bind;
                    buttonSecondaryAttack.enabled = true;
                    break;
                case "TERTIARY_ATTACK":
                    buttonTertiaryAttack.text = bind;
                    buttonTertiaryAttack.enabled = true;
                    break;
                case "GRENADE":
                    buttonGrenade.text = bind;
                    buttonGrenade.enabled = true;
                    break;
                case "RELOAD":
                    buttonReload.text = bind;
                    buttonReload.enabled = true;
                    break;
                case "QUICK_SWAP":
                    buttonQuickSwap.text = bind;
                    buttonQuickSwap.enabled = true;
                    break;
                case "MOVE_FORWARD":
                    buttonMoveForward.text = bind;
                    buttonMoveForward.enabled = true;
                    break;
                case "MOVE_BACK":
                    buttonMoveBack.text = bind;
                    buttonMoveBack.enabled = true;
                    break;
                case "MOVE_LEFT":
                    buttonMoveLeft.text = bind;
                    buttonMoveLeft.enabled = true;
                    break;
                case "MOVE_RIGHT":
                    buttonMoveRight.text = bind;
                    buttonMoveRight.enabled = true;
                    break;
                case "JUMP":
                    buttonJump.text = bind;
                    buttonJump.enabled = true;
                    break;
                case "CONSOLE":
                    buttonConsole.text = bind;
                    buttonConsole.enabled = true;
                    break;
                case "CROUCH":
                    buttonCrouch.text = bind;
                    buttonCrouch.enabled = true;
                    break;
                case "SPRINT":
                    buttonSprint.text = bind;
                    buttonSprint.enabled = true;
                    break;
                case "INTERACT":
                    buttonInteract.text = bind;
                    buttonInteract.enabled = true;
                    break;
                case "FLASHLIGHT":
                    buttonFlashlight.text = bind;
                    buttonFlashlight.enabled = true;
                    break;
                case "COVER_MOUTH":
                    buttonCoverMouth.text = bind;
                    buttonCoverMouth.enabled = true;
                    break;
                case "QUICK_SAVE":
                    buttonQuickSave.text = bind;
                    buttonQuickSave.enabled = true;
                    break;
                case "QUICK_LOAD":
                    buttonQuickLoad.text = bind;
                    buttonQuickLoad.enabled = true;
                    break;
                case "NOCLIP":
                    buttonNoclip.text = bind;
                    buttonNoclip.enabled = true;
                    break;
                case "VIEWM_INSPECT":
                    buttonViewmodelInspect.text = bind;
                    buttonViewmodelInspect.enabled = true;
                    break;
                case "ZOOM":
                    buttonZoom.text = bind;
                    buttonZoom.enabled = true;
                    break;
            }
        }

        function onConvarLoaded(convar, value) {
            switch (convar) {
                case "snd_gain":
                    masterVolume.value = value * 100;
                    break;
                case "snd_gamevolume":
                    gameVolume.value = value * 100;
                    break;
                case "snd_musicvolume":
                    combatMusicVolume.value = value * 100;
                    break;
                case "snd_gamevoicevolume":
                    characterVoiceVolume.value = value * 100;
                    break;
                case "mouse_pitchyaw_sensitivity":
                    mouseSensitivity.value = value;
                    break;
                case "fov_desired":
                    fov.value = value;
                    break;
                case "skill":
                    difficultyLayout.children[parseInt(value) * 2].checked = true;
                    break;
                case "r_light_sensitivity_mode":
                    lightSensitivityMode.checked = value === "1";
                    break;
                case "hlvr_closed_caption_type":
                    subtitles.currentIndex = parseInt(value);
                    break;
            }
        }
    }

    ListModel {
        id: savesModel
    }

    ListView {
        id: savesListView
        anchors.top: menu.top
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: 400
        spacing: 5
        model: savesModel
        delegate: Button {
            width: 400
            contentItem: Column {
                Text {
                    width: parent.width
                    font.bold: saveFileName !== "cancel"
                    text: saveName
                    horizontalAlignment: (saveFileName === "cancel") ? Text.AlignHCenter : Text.AlignLeft
                }
                Text {
                    visible: saveFileName !== "cancel"
                    text: saveDateTime
                    horizontalAlignment: Text.AlignLeft
                }
            }
            onClicked: gameMenu.loadSave(saveFileName)
        }
    }

    ListModel {
        id: chaptersModel

        ListElement { chapterNumber: ""; chapterName: "Cancel"; chapterMapName: "cancel"; addonMap: false; mapEnabled: true; }
        ListElement { chapterNumber: ""; chapterName: "Addon Maps"; chapterMapName: "addon_maps"; addonMap: false; mapEnabled: true; }
        ListElement { chapterNumber: "Chapter 1"; chapterName: "Entanglement"; chapterMapName: "a1_intro_world"; addonMap: false; mapEnabled: true; }
        ListElement { chapterNumber: "Chapter 2"; chapterName: "The Quarantine Zone"; chapterMapName: "a2_quarantine_entrance"; addonMap: false; mapEnabled: true; }
        ListElement { chapterNumber: "Chapter 3"; chapterName: "Is or Will Be"; chapterMapName: "a2_headcrabs_tunnel"; addonMap: false; mapEnabled: true; }
        ListElement { chapterNumber: "Chapter 4"; chapterName: "Superweapon"; chapterMapName: "a3_station_street"; addonMap: false; mapEnabled: true; }
        ListElement { chapterNumber: "Chapter 5"; chapterName: "The Northern Star"; chapterMapName: "a3_hotel_lobby_basement"; addonMap: false; mapEnabled: true; }
        ListElement { chapterNumber: "Chapter 6"; chapterName: "Arms Race"; chapterMapName: "a3_c17_processing_plant"; addonMap: false; mapEnabled: true; }
        ListElement { chapterNumber: "Chapter 7"; chapterName: "Jeff"; chapterMapName: "a3_distillery"; addonMap: false; mapEnabled: true; }
        ListElement { chapterNumber: "Chapter 8"; chapterName: "Captivity"; chapterMapName: "a4_c17_zoo"; addonMap: false; mapEnabled: true; }
        ListElement { chapterNumber: "Chapter 9"; chapterName: "Revelations"; chapterMapName: "a4_c17_tanker_yard"; addonMap: false; mapEnabled: true; }
        ListElement { chapterNumber: "Chapter 10"; chapterName: "Breaking and Entering"; chapterMapName: "a4_c17_water_tower"; addonMap: false; mapEnabled: true; }
        ListElement { chapterNumber: "Chapter 11"; chapterName: "Point Extraction"; chapterMapName: "a5_vault"; addonMap: false; mapEnabled: true; }
    }

    ListModel {
        id: addonMapsModel
    }

    ListView {
        id: chaptersListView
        visible: false
        anchors.top: menu.top
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: 400
        spacing: 5
        model: chaptersModel
        delegate: Button {
            enabled: mapEnabled
            width: 400
            contentItem: Column {
                Text {
                    visible: chapterNumber !== ""
                    width: parent.width
                    font.bold: true
                    text: chapterNumber
                }
                Text {
                    width: parent.width
                    text: chapterName
                    horizontalAlignment: (chapterNumber === "") ? Text.AlignHCenter : Text.AlignLeft
                }
            }
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
        width: 400
        spacing: 5
        anchors.horizontalCenter: parent.horizontalCenter
        model: addonsModel
        delegate: Button {
            width: 400
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
            buttonSaveGame.visible = false;
            buttonMainMenu.visible = false;
            labelHudHealth.visible = false;
        }
    }

    Column {
        id: menu
        visible: false
        width: 200
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -60
        spacing: 5

        Button {
            id: buttonPlay
            width: 200
            height: 34
            text: qsTr("Continue")
            Layout.fillWidth: true
            onClicked: gameMenu.buttonPlayClicked();
        }

        Button {
            id: buttonSaveGame
            width: 200
            height: 34
            text: qsTr("Save Game")
            Layout.fillWidth: true
            onClicked: gameMenu.buttonSaveGameClicked();
        }

        Button {
            id: buttonLoadGame
            width: 200
            height: 34
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
            width: 200
            height: 34
            text: qsTr("New Game")
            Layout.fillWidth: true
            onClicked: gameMenu.buttonNewGameClicked();
        }

        Button {
            id: buttonOptions
            width: 200
            height: 34
            text: qsTr("Options")
            Layout.fillWidth: true
            onClicked: function() {
                menu.visible = false;
                options.visible = true;
                gameMenu.buttonOptionsClicked();
            }
        }

        Button {
            id: buttonMainMenu
            width: 200
            height: 34
            text: qsTr("Main Menu")
            Layout.fillWidth: true
            onClicked: gameMenu.buttonMainMenuClicked()
        }

        Button {
            id: buttonAddons
            width: 200
            height: 34
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
            width: 200
            height: 34
            text: qsTr("Quit")
            Layout.fillWidth: true
            onClicked: gameMenu.buttonQuitClicked()
        }
    }

    Rectangle {
        property var apply: new Map()

        id: options
        visible: false
        anchors.fill: parent
        color: "#CC000000"

        ColumnLayout {
            anchors.fill: parent

            Item {
                height: childrenRect.height
                Layout.fillWidth: true

                TabBar {
                    id: tabBar
                    width: 600
                    anchors.horizontalCenter: parent.horizontalCenter

                    TabButton {
                        id: tabDifficulty
                        text: qsTr("Difficulty")
                    }

                    TabButton {
                        id: tabMouse
                        text: qsTr("Mouse")
                    }

                    TabButton {
                        id: tabInputMapping
                        text: qsTr("Input Mapping")
                    }

                    TabButton {
                        id: tabAudio
                        text: qsTr("Audio")
                    }

                    TabButton {
                        id: tabVideo
                        text: qsTr("Video")
                    }
                }

                Button {
                    anchors.right: parent.right
                    width: 100
                    text: qsTr("OK")
                    onClicked: function() {
                        options.visible = false;
                        menu.visible = true;
                        gameMenu.changeOptions(Array.from(options.apply, ([key, value]) => key + " " + value));
                        options.apply.clear()
                    }
                }
            }

            StackLayout {
                currentIndex: tabBar.currentIndex

                Item {
                    id: difficulty

                    GridLayout {
                        id: difficultyLayout
                        anchors.horizontalCenter: parent.horizontalCenter
                        columns: 2

                        RadioButton {
                            id: storyDifficulty
                            onClicked: options.apply.set("skill", 0)
                        }

                        Label {
                            color: "white"
                            text: qsTr("Story")
                            Layout.topMargin: -10
                        }

                        RadioButton {
                            id: easyDifficulty
                            onClicked: options.apply.set("skill", 1)
                        }

                        Label {
                            color: "white"
                            text: qsTr("Easy")
                            Layout.topMargin: -10
                        }

                        RadioButton {
                            id: normalDifficulty
                            onClicked: options.apply.set("skill", 2)
                        }

                        Label {
                            color: "white"
                            text: qsTr("Normal")
                            Layout.topMargin: -10
                        }

                        RadioButton {
                            id: hardDifficulty
                            onClicked: options.apply.set("skill", 3)
                        }

                        Label {
                            color: "white"
                            text: qsTr("Hard")
                            Layout.topMargin: -10
                        }
                    }
                }

                Item {
                    id: mouse

                    GridLayout {
                        anchors.horizontalCenter: parent.horizontalCenter
                        columns: 3

                        Label {
                            color: "white"
                            text: qsTr("Mouse Sensitivity")
                            Layout.alignment: Qt.AlignRight
                        }

                        Slider {
                            id: mouseSensitivity
                            from: 1
                            to: 100
                            value: 50
                            stepSize: 1
                            snapMode: Slider.SnapAlways
                            Layout.preferredWidth: 220
                            onValueChanged: options.apply.set("mouse_pitchyaw_sensitivity", value)
                        }

                        Label {
                            color: "white"
                            text: mouseSensitivity.value
                            Layout.preferredWidth: 20
                        }

                        /*Label {
                            color: "white"
                            text: qsTr("Horizontal Inverted")
                            Layout.alignment: Qt.AlignRight
                        }

                        CheckBox {
                            id: mouseInvertX
                        }

                        Item {}*/

                        Label {
                            color: "white"
                            text: qsTr("Vertical Inverted")
                            Layout.alignment: Qt.AlignRight
                        }

                        CheckBox {
                            id: mouseInvertY
                            onToggled: options.apply.set("mouse_invert_y", checked)
                        }

                        Item {}
                    }
                }

                Item {
                    id: inputMapping

                    GridLayout {
                        anchors.horizontalCenter: parent.horizontalCenter
                        columns: 4

                        Label {
                            color: "white"
                            text: qsTr("Warning: Changes are only applied after loading a save or starting a new game!")
                            Layout.columnSpan: parent.columns
                        }

                        Label {
                            color: "white"
                            text: qsTr("Primary Attack")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonPrimaryAttack
                            text: "MOUSE1"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("PRIMARY_ATTACK")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Grenade")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonGrenade
                            text: "G"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("GRENADE")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Secondary Attack")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonSecondaryAttack
                            text: "MOUSE2"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("SECONDARY_ATTACK")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Reload")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonReload
                            text: "R"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("RELOAD")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Tertiary Attack")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonTertiaryAttack
                            text: "MOUSE3"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("TERTIARY_ATTACK")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Quick Swap")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonQuickSwap
                            text: "Q"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("QUICK_SWAP")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Move Forward")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonMoveForward
                            text: "W"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("MOVE_FORWARD")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Jump")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonJump
                            text: "SPACE"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("JUMP")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Move Back")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonMoveBack
                            text: "S"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("MOVE_BACK")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Console")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonConsole
                            text: "C"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("CONSOLE")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Move Left")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonMoveLeft
                            text: "A"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("MOVE_LEFT")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Crouch")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonCrouch
                            text: "CTRL"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("CROUCH")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Move Right")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonMoveRight
                            text: "D"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("MOVE_RIGHT")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Sprint")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonSprint
                            text: "SHIFT"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("SPRINT")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Interact")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonInteract
                            text: "E"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("INTERACT")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Flashlight")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonFlashlight
                            text: "F"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("FLASHLIGHT")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Cover Mouth")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonCoverMouth
                            text: "H"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("COVER_MOUTH")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Inspect Weapon")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonViewmodelInspect
                            text: "T"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("VIEWM_INSPECT")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Quick Save")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonQuickSave
                            text: "F5"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("QUICK_SAVE")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Noclip")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonNoclip
                            text: "V"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("NOCLIP")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Quick Load")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonQuickLoad
                            text: "F9"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("QUICK_LOAD")
                            }
                        }

                        Label {
                            color: "white"
                            text: qsTr("Zoom")
                            Layout.alignment: Qt.AlignRight
                        }

                        Button {
                            id: buttonZoom
                            text: "Z"
                            Layout.preferredWidth: 100
                            onClicked: function() {
                                enabled = false;
                                text = "...";
                                gameMenu.recordInput("ZOOM")
                            }
                        }
                    }
                }

                Item {
                    id: audio

                    GridLayout {
                        anchors.horizontalCenter: parent.horizontalCenter
                        columns: 3

                        Label {
                            color: "white"
                            text: qsTr("Master Volume")
                            Layout.alignment: Qt.AlignRight
                        }

                        Slider {
                            id: masterVolume
                            from: 0
                            to: 100
                            value: 100
                            stepSize: 1
                            snapMode: Slider.SnapAlways
                            Layout.preferredWidth: 220
                            onValueChanged: options.apply.set("snd_gain", value / 100)
                        }

                        Label {
                            color: "white"
                            text: masterVolume.value
                            Layout.preferredWidth: 20
                        }

                        Label {
                            color: "white"
                            text: qsTr("Game")
                            Layout.alignment: Qt.AlignRight
                        }

                        Slider {
                            id: gameVolume
                            from: 0
                            to: 100
                            value: 100
                            stepSize: 1
                            snapMode: Slider.SnapAlways
                            Layout.preferredWidth: 220
                            onValueChanged: options.apply.set("snd_gamevolume", value / 100)
                        }

                        Label {
                            color: "white"
                            text: gameVolume.value
                            Layout.preferredWidth: 20
                        }

                        Label {
                            color: "white"
                            text: qsTr("Combat Music")
                            Layout.alignment: Qt.AlignRight
                        }

                        Slider {
                            id: combatMusicVolume
                            from: 0
                            to: 100
                            value: 100
                            stepSize: 1
                            snapMode: Slider.SnapAlways
                            Layout.preferredWidth: 220
                            onValueChanged: options.apply.set("snd_musicvolume", value / 100)
                        }

                        Label {
                            color: "white"
                            text: combatMusicVolume.value
                            Layout.preferredWidth: 20
                        }

                        Label {
                            color: "white"
                            text: qsTr("Character Voice")
                            Layout.alignment: Qt.AlignRight
                        }

                        Slider {
                            id: characterVoiceVolume
                            from: 0
                            to: 100
                            value: 100
                            stepSize: 1
                            snapMode: Slider.SnapAlways
                            Layout.preferredWidth: 220
                            onValueChanged: options.apply.set("snd_gamevoicevolume", value / 100)
                        }

                        Label {
                            color: "white"
                            text: characterVoiceVolume.value
                            Layout.preferredWidth: 20
                        }

                        Label {
                            color: "white"
                            text: qsTr("Subtitles")
                            Layout.alignment: Qt.AlignRight
                        }

                        ComboBox {
                            id: subtitles
                            model: ["Subtitles Off", "Subtitles On", "Subtitles and Closed Captions"]
                            Layout.preferredWidth: 220
                            onActivated: options.apply.set("hlvr_closed_caption_type", currentIndex)
                        }
                    }
                }

                Item {
                    id: video

                    GridLayout {
                        anchors.horizontalCenter: parent.horizontalCenter
                        columns: 3

                        Label {
                            color: "white"
                            text: qsTr("Change Graphics Preset")
                            Layout.alignment: Qt.AlignRight
                        }

                        ComboBox {
                            id: graphicsPreset
                            model: ["Unchanged", "Low", "Medium", "High", "Ultra"]
                            onActivated: options.apply.set("set_video_config_level", currentIndex - 1)
                        }

                        Item {}

                        Label {
                            color: "white"
                            text: qsTr("Reduce strength and flickering of lights")
                            Layout.alignment: Qt.AlignRight
                        }

                        CheckBox {
                            id: lightSensitivityMode
                            onToggled: options.apply.set("r_light_sensitivity_mode", checked ? 1 : 0)
                        }

                        Item {}

                        Label {
                            color: "white"
                            text: qsTr("FOV")
                            Layout.alignment: Qt.AlignRight
                        }

                        Slider {
                            id: fov
                            from: 60
                            to: 110
                            value: 80
                            stepSize: 1
                            snapMode: Slider.SnapAlways
                            Layout.preferredWidth: 220
                            onValueChanged: options.apply.set("fov_desired", value)
                        }

                        Label {
                            color: "white"
                            text: fov.value
                            Layout.preferredWidth: 20
                        }
                    }
                }
            }
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
