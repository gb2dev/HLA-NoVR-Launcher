class_name GameMenu
extends Window


signal input_entered(input: String)

const GAME_MENU_THEME = preload("res://game_menu_theme.tres")
const CHAPTER_MAPS = [
	"a1_intro_world",
	"a2_quarantine_entrance",
	"a2_headcrabs_tunnel",
	"a3_station_street",
	"a3_hotel_lobby_basement",
	"a3_c17_processing_plant",
	"a3_distillery",
	"a4_c17_zoo",
	"a4_c17_tanker_yard",
	"a4_c17_water_tower",
	"a5_vault",
]

@onready var content: Control = $Content
@onready var vbox_main_menu: VBoxContainer = $Content/VBoxContainerMainMenu
@onready var vbox_select_chapter: VBoxContainer = $Content/VBoxContainerSelectChapter
@onready var vbox_game_slot_buttons: VBoxContainer = $Content/VBoxContainerGames/ScrollContainer/VBoxContainerButtons
@onready var vbox_load_game_slot: VBoxContainer = $Content/VBoxContainerLoadGameSlot
@onready var vbox_games: VBoxContainer = $Content/VBoxContainerGames
@onready var label_game_slot_title: Label = $Content/VBoxContainerLoadGameSlot/ColorRectTopBar/LabelTitle
@onready var vbox_select_chapter_buttons: VBoxContainer = $Content/VBoxContainerSelectChapter/ScrollContainer/VBoxContainer/VBoxContainerButtons
@onready var vbox_start_new_game: VBoxContainer = $Content/VBoxContainerStartNewGame
@onready var button_difficulty: Button = $Content/VBoxContainerStartNewGame/ScrollContainer/VBoxContainer/ButtonDifficulty
@onready var button_dev_commentary: Button = $Content/VBoxContainerStartNewGame/ScrollContainer/VBoxContainer/ButtonDevCommentary
@onready var label_mouse_sensitivity: Label = $Content/VBoxContainerOptionsMouse/ScrollContainer/VBoxContainer/MarginContainerMouse/VBoxContainer/HBoxContainer/LabelMouseSensitivity
@onready var button_invert_x: Button = $Content/VBoxContainerOptionsMouse/ScrollContainer/VBoxContainer/ButtonInvertX
@onready var button_invert_y: Button = $Content/VBoxContainerOptionsMouse/ScrollContainer/VBoxContainer/ButtonInvertY
@onready var label_master_volume: Label = $Content/VBoxContainerOptionsAudio/ScrollContainer/VBoxContainer/MarginContainerMasterVolume/VBoxContainer/HBoxContainer/LabelMasterVolume
@onready var label_game_volume: Label = $Content/VBoxContainerOptionsAudio/ScrollContainer/VBoxContainer/MarginContainerGameVolume/VBoxContainer/HBoxContainer/LabelGameVolume
@onready var label_combat_music_volume: Label = $Content/VBoxContainerOptionsAudio/ScrollContainer/VBoxContainer/MarginContainerCombatMusicVolume/VBoxContainer/HBoxContainer/LabelCombatMusicVolume
@onready var label_character_voice_volume: Label = $Content/VBoxContainerOptionsAudio/ScrollContainer/VBoxContainer/MarginContainerCharacterVoiceVolume/VBoxContainer/HBoxContainer/LabelCharacterVoiceVolume
@onready var button_subtitles: Button = $Content/VBoxContainerOptionsAudio/ScrollContainer/VBoxContainer/ButtonSubtitles
@onready var button_graphics_preset: Button = $Content/VBoxContainerOptionsVideo/ScrollContainer/VBoxContainer/ButtonGraphicsPreset
@onready var button_light_sensitivity_mode: Button = $Content/VBoxContainerOptionsVideo/ScrollContainer/VBoxContainer/ButtonLightSensitivityMode
@onready var vbox_input_mapping_buttons: VBoxContainer = $Content/VBoxContainerOptionsInputMapping/ScrollContainer/VBoxContainerButtons
@onready var label_fov: Label = $Content/VBoxContainerOptionsVideo/ScrollContainer/VBoxContainer/MarginContainerFOV/VBoxContainer/HBoxContainer/LabelFOV
@onready var slider_master_volume: HSlider = $Content/VBoxContainerOptionsAudio/ScrollContainer/VBoxContainer/MarginContainerMasterVolume/VBoxContainer/HBoxContainer/HSliderMasterVolume
@onready var slider_game_volume: HSlider = $Content/VBoxContainerOptionsAudio/ScrollContainer/VBoxContainer/MarginContainerGameVolume/VBoxContainer/HBoxContainer/HSliderGameVolume
@onready var slider_combat_music_volume: HSlider = $Content/VBoxContainerOptionsAudio/ScrollContainer/VBoxContainer/MarginContainerCombatMusicVolume/VBoxContainer/HBoxContainer/HSliderCombatMusicVolume
@onready var slider_character_voice_volume: HSlider = $Content/VBoxContainerOptionsAudio/ScrollContainer/VBoxContainer/MarginContainerCharacterVoiceVolume/VBoxContainer/HBoxContainer/HSliderCharacterVoiceVolume
@onready var slider_mouse_sensitivity: HSlider = $Content/VBoxContainerOptionsMouse/ScrollContainer/VBoxContainer/MarginContainerMouse/VBoxContainer/HBoxContainer/HSliderMouseSensitivity
@onready var slider_fov: HSlider = $Content/VBoxContainerOptionsVideo/ScrollContainer/VBoxContainer/MarginContainerFOV/VBoxContainer/HBoxContainer/HSliderFOV
@onready var vbox_load_game_slot_buttons: VBoxContainer = $Content/VBoxContainerLoadGameSlot/ScrollContainer/VBoxContainerButtons
@onready var vbox_start_new_game_slot: VBoxContainer = $Content/VBoxContainerStartNewGameSlot
@onready var vbox_start_new_game_slot_buttons: VBoxContainer = $Content/VBoxContainerStartNewGameSlot/ScrollContainer/VBoxContainerButtons
@onready var vbox_options: VBoxContainer = $Content/VBoxContainerOptions
@onready var button_continue: Button = $Content/VBoxContainerMainMenu/ButtonContinue

var launcher: Launcher
var remapping_input := false
var chapters_unlocked: int = 11 # TODO
var difficulty := 2
var dev_commentary := false
var invert_x := false
var invert_y := false
var subtitles := 0
var graphics_preset := 0
var light_sensitivity_mode := false
var selected_chapter: int
var pause_menu_mode := false


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	for button: Button in vbox_game_slot_buttons.get_children():
		button.pressed.connect(func():
			vbox_load_game_slot.show()
			vbox_games.hide()
			var slot := button.get_index()
			label_game_slot_title.text = label_game_slot_title.text.left(-1) + str(slot)
			send_game_command("save_set_subdirectory s%s" % slot)
			retrieve_save_files("s%s" % slot)
		)
	for button: Button in vbox_start_new_game_slot_buttons.get_children():
		button.pressed.connect(func():
			send_game_command("save_set_subdirectory s%s;addon_play %s" % [button.get_index(), CHAPTER_MAPS[selected_chapter]])
		)
	for button: Button in vbox_select_chapter_buttons.get_children():
		button.pressed.connect(func():
			vbox_start_new_game.show()
			vbox_select_chapter.hide()
			selected_chapter = button.get_index()
		)
	var bindings := FileAccess.open(launcher.installation_path + "/game/hlvr/scripts/vscripts/bindings.lua", FileAccess.READ)
	if bindings:
		while not bindings.eof_reached():
			var line := bindings.get_line()
			if line.is_empty():
				break
			line = line.replace(" ", "").replace("\"", "")
			var binding := line.split("=")
			if binding[0] not in ["MOUSE_SENSITIVITY",  "INVERT_MOUSE_X", "INVERT_MOUSE_Y", "FOV", "PAUSE", "MAIN_MENU"]:
				var button := Button.new()
				button.add_to_group(&"InputButton")
				button.text = tr(binding[0]) + ": " + binding[1]
				vbox_input_mapping_buttons.add_child(button)
				button.pressed.connect(func():
					get_tree().set_group(&"InputButton", &"disabled", true)
					remapping_input = true
					launcher.label_info.text = "Please press any key/button..."
					always_on_top = false
					OS.execute(launcher.launcher_helper_executable_name, ["focuslauncher"], [])
					input_entered.connect(func(input: String):
						hide()
						always_on_top = true
						show()
						get_tree().set_group(&"InputButton", "disabled", false)
						remapping_input = false
						button.text = tr(binding[0]) + ": " + input
						write_to_bindings_file(binding[0], input)
					, CONNECT_ONE_SHOT)
				)
	read_convars_file()
	read_bindings_file()
	read_save_cfg_file()
	var thread := Thread.new()
	thread.start(_thread_game_output)


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass


func _input(event: InputEvent) -> void:
	if event is InputEventMouseButton and event.is_pressed():
		OS.execute(launcher.launcher_helper_executable_name, ["focusgame"], [])


func _thread_game_output() -> void:
	var file := FileAccess.open(launcher.installation_path + "/game/hlvr/console.log", FileAccess.WRITE_READ)
	var file_pos := 0
	while true:
		file.seek(file_pos)
		var line = file.get_line()
		file_pos = file.get_position()
		if not line.is_empty():
			var command: PackedStringArray
			if line.contains("CHostStateMgr::QueueNewRequest( Loading (") or line.contains("CHostStateMgr::QueueNewRequest( Restoring Save ("):
				command = ["", "hide"]
			else:
				command = line.split("[GameMenu] ")
			if command.size() > 1:
				command = command[1].split(" ")
				receive_menu_command.bind(command).call_deferred()


func retrieve_save_files(slot: String) -> void:
	var dir = DirAccess.open(launcher.installation_path + "/game/hlvr/SAVE/" + slot)
	for button: Button in vbox_load_game_slot_buttons.get_children():
		button.queue_free()
	if dir:
		dir.list_dir_begin()
		var file_name = dir.get_next()
		while file_name != "":
			if not dir.current_is_dir():
				if file_name.ends_with(".sav"):
					var button := Button.new()
					var save_name := file_name.trim_suffix(".sav")
					button.text = save_name.capitalize()
					vbox_load_game_slot_buttons.add_child(button)
					button.pressed.connect(func():
						send_game_command("load %s" % save_name)
					)
			file_name = dir.get_next()


func receive_menu_command(command: PackedStringArray) -> void:
	if command[0] == "main_menu_mode":
		pause_menu_mode = false
		await get_tree().create_timer(5.0).timeout
		# TODO: Continue in main menu should load most recent save
		button_continue.visible = false
		content.visible = true
		for control: Control in content.get_children():
			control.visible = false
		vbox_main_menu.visible = true
		visible = true
	elif command[0] == "hide":
		# Pause Menu Mode
		pause_menu_mode = true
		button_continue.visible = true
		visible = false
	elif command[0] == "give_achievement":
		OS.execute("SAM.Game" if launcher.os_platform_unix else "SAM.Game.exe", ["546560", command[1]], [])


func send_game_command(command: String) -> void:
	send_game_script("SendToConsole(\"" + command + "\")")


func send_game_script(content: String) -> void:
	var menu_exec := FileAccess.open(launcher.installation_path + "/game/hlvr/scripts/vscripts/main_menu_exec.lua", FileAccess.WRITE)
	if menu_exec:
		menu_exec.store_string(content)
		menu_exec.close()
		OS.execute(launcher.launcher_helper_executable_name, ["exec"], [])


func write_to_bindings_file(key: String, value: Variant) -> void:
	var bindings := FileAccess.open(launcher.installation_path + "/game/hlvr/scripts/vscripts/bindings.lua", FileAccess.READ_WRITE)
	if bindings:
		var content: String
		while not bindings.eof_reached():
			var line := bindings.get_line()
			if line.is_empty():
				break
			if line.begins_with(key):
				var line_split := line.split("=")
				if typeof(value) == TYPE_STRING:
					value = "\"%s\"" % value
				line = line_split[0] + "= %s" % value
			content += line + "\n"
		bindings.resize(0)
		bindings.seek(0)
		bindings.store_string(content)
		bindings.close()


func write_to_save_cfg_file(key: String, value: Variant) -> void:
	var save_cfg := FileAccess.open(launcher.installation_path + "/game/hlvr/SAVE/personal.cfg", FileAccess.READ_WRITE)
	if save_cfg:
		var content: String
		while not save_cfg.eof_reached():
			var line := save_cfg.get_line()
			if line.is_empty():
				break
			if line.contains(key):
				var regex := RegEx.new()
				regex.compile("(.*)\".*\"")
				var result := regex.search(line)
				line = result.get_string(1) + "\"%s\"" % value
			content += line + "\n"
		save_cfg.resize(0)
		save_cfg.seek(0)
		save_cfg.store_string(content)
		save_cfg.close()


func read_save_cfg_file() -> void:
	var save_cfg := FileAccess.open(launcher.installation_path + "/game/hlvr/SAVE/personal.cfg", FileAccess.READ)
	if save_cfg:
		var regex := RegEx.new()
		regex.compile("\"(.*)\"[ \t]*\"(.*)\"")
		for result in regex.search_all(save_cfg.get_as_text()):
			var key := result.get_string(1)
			var value := result.get_string(2)
			match key:
				"setting.skill":
					for i in (int(value) + 2) % 4:
						_on_button_difficulty_pressed()
				"setting.commentary":
					if value == "1":
						_on_button_dev_commentary_pressed()


func read_convars_file() -> void:
	var convars := FileAccess.open(launcher.installation_path + "/game/hlvr/cfg/machine_convars.vcfg", FileAccess.READ)
	if convars:
		var regex := RegEx.new()
		regex.compile("\"(.*)\"[ \t]*\"(.*)\"")
		for result in regex.search_all(convars.get_as_text()):
			var key := result.get_string(1)
			var value := result.get_string(2)
			match key:
				"snd_gain":
					slider_master_volume.value = float(value)
				"snd_gamevolume":
					slider_game_volume.value = float(value)
				"snd_musicvolume":
					slider_combat_music_volume.value = float(value)
				"snd_gamevoicevolume":
					slider_character_voice_volume.value = float(value)
				"fov_desired":
					slider_fov.value = float(value)
				"r_light_sensitivity_mode":
					if value == "true":
						_on_button_light_sensitivity_mode_pressed()
				"hlvr_closed_caption_type":
					for i in int(value):
						_on_button_subtitles_pressed()


func read_bindings_file() -> void:
	var bindings := FileAccess.open(launcher.installation_path + "/game/hlvr/scripts/vscripts/bindings.lua", FileAccess.READ)
	if bindings:
		var regex := RegEx.new()
		regex.compile("(.*)=(.*)")
		for result in regex.search_all(bindings.get_as_text()):
			var key := result.get_string(1).strip_edges()
			var value := result.get_string(2).strip_edges()
			match key:
				"MOUSE_SENSITIVITY":
					slider_mouse_sensitivity.value = float(value)
				"INVERT_MOUSE_X":
					if value == "true":
						_on_button_invert_x_pressed()
				"INVERT_MOUSE_Y":
					if value == "true":
						_on_button_invert_y_pressed()


func toggle_pause_menu():
	if visible:
		send_game_command("gameui_allowescape;gameui_preventescapetoshow;gameui_hide;r_drawvgui 1;unpause")
		visible = false
		content.visible = false
	else:
		# Go back to root pause menu layer
		vbox_main_menu.show()
		for vbox: VBoxContainer in content.get_children():
			if vbox != vbox_main_menu:
				vbox.hide()

		send_game_command("gameui_preventescape;gameui_allowescapetoshow;gameui_activate;r_drawvgui 0;pause")
		visible = true
		content.visible = true


func _on_button_continue_pressed() -> void:
	if pause_menu_mode:
		toggle_pause_menu()
	else:
		# TODO: Implement proper loading system
		send_game_command("load quick")


func _on_button_start_new_game_pressed() -> void:
	if chapters_unlocked > 0:
		vbox_select_chapter.show()
		vbox_main_menu.hide()


func _on_button_quit_game_pressed() -> void:
	send_game_command("quit")


func _on_button_difficulty_pressed() -> void:
	difficulty = (difficulty + 1) % 4
	button_difficulty.text = "Difficulty: %s" % ["Story", "Easy", "Normal", "Hard"][difficulty]
	write_to_save_cfg_file("setting.skill", difficulty)


func _on_button_dev_commentary_pressed() -> void:
	dev_commentary = not dev_commentary
	button_dev_commentary.text = "Developer Commentary: %s" % ("ON" if dev_commentary else "OFF")
	write_to_save_cfg_file("setting.commentary", int(dev_commentary))


func _on_slider_mouse_sensitivity_value_changed(value: float) -> void:
	label_mouse_sensitivity.text = "%s" % value
	write_to_bindings_file("MOUSE_SENSITIVITY", value)
	send_game_command("mouse_pitchyaw_sensitivity %s" % value)


func _on_button_invert_x_pressed() -> void:
	invert_x = not invert_x
	button_invert_x.text = "Horizontal Inverted: %s" % ("ON" if invert_x else "OFF")
	write_to_bindings_file("INVERT_MOUSE_X", invert_x)


func _on_button_invert_y_pressed() -> void:
	invert_y = not invert_y
	button_invert_y.text = "Vertical Inverted: %s" % ("ON" if invert_y else "OFF")
	write_to_bindings_file("INVERT_MOUSE_Y", invert_y)


func _on_button_subtitles_pressed() -> void:
	subtitles = (subtitles + 1) % 3
	button_subtitles.text = "Subtitles: %s" % ["SUBTITLES OFF", "SUBTITLES ON", "CLOSED CAPTIONS"][subtitles]
	send_game_command("hlvr_closed_caption_type %s" % subtitles)


func _on_slider_master_volume_value_changed(value: float) -> void:
	label_master_volume.text = "%.2f" % value
	send_game_command("snd_gain %s" % value)


func _on_slider_game_volume_value_changed(value: float) -> void:
	label_game_volume.text = "%.2f" % value
	send_game_command("snd_gamevolume %s" % value)


func _on_slider_combat_music_volume_value_changed(value: float) -> void:
	label_combat_music_volume.text = "%.2f" % value
	send_game_command("snd_musicvolume %s" % value)


func _on_slider_character_voice_volume_value_changed(value: float) -> void:
	label_character_voice_volume.text = "%.2f" % value
	send_game_command("snd_gamevoicevolume %s" % value)


func _on_slider_fov_value_changed(value: float) -> void:
	label_fov.text = "%s" % value
	write_to_bindings_file("FOV", int(value))
	send_game_command("fov_desired %s" % int(value))


func _on_button_graphics_preset_pressed() -> void:
	graphics_preset = (graphics_preset + 1) % 5
	button_graphics_preset.text = "Graphics Preset: %s" % ["DEFAULT", "LOW", "MEDIUM", "HIGH", "ULTRA"][graphics_preset]
	if graphics_preset > 0:
		send_game_command("set_video_config_level %s" % (graphics_preset - 1))
	else:
		send_game_command("video_config_autoconfig_reset")


func _on_button_light_sensitivity_mode_pressed() -> void:
	light_sensitivity_mode = not light_sensitivity_mode
	button_light_sensitivity_mode.text = "Reduce Light Flickering: %s" % ("ON" if light_sensitivity_mode else "OFF")
	send_game_command("r_light_sensitivity_mode %s" % light_sensitivity_mode)


func _on_button_start_game_pressed() -> void:
	vbox_start_new_game_slot.show()
