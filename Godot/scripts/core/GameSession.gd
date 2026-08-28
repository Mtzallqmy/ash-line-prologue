extends Node

signal mission_started
signal mission_finished(victory: bool)

const MENU_SCENE := "res://scenes/MainMenu.tscn"
const ARENA_SCENE := "res://scenes/CombatArena.tscn"

var last_result := ""

func _ready() -> void:
	_ensure_input_map()

func start_mission() -> void:
	last_result = ""
	get_tree().change_scene_to_file(ARENA_SCENE)
	mission_started.emit()

func restart_mission() -> void:
	get_tree().reload_current_scene()

func return_to_menu() -> void:
	get_tree().change_scene_to_file(MENU_SCENE)

func finish_mission(victory: bool) -> void:
	last_result = "VICTORY" if victory else "MISSION FAILED"
	mission_finished.emit(victory)

func _ensure_input_map() -> void:
	_bind_key("move_forward", KEY_W)
	_bind_key("move_forward", KEY_UP)
	_bind_key("move_backward", KEY_S)
	_bind_key("move_backward", KEY_DOWN)
	_bind_key("move_left", KEY_A)
	_bind_key("move_left", KEY_LEFT)
	_bind_key("move_right", KEY_D)
	_bind_key("move_right", KEY_RIGHT)
	_bind_key("jump", KEY_SPACE)
	_bind_key("sprint", KEY_SHIFT)
	_bind_key("crouch", KEY_C)
	_bind_key("fire", KEY_F)
	_bind_key("reload", KEY_R)
	_bind_key("aim", KEY_Q)
	_bind_key("pause_menu", KEY_ESCAPE)

func _bind_key(action: StringName, physical_key: Key) -> void:
	if not InputMap.has_action(action):
		InputMap.add_action(action)
	var event := InputEventKey.new()
	event.physical_keycode = physical_key
	for existing in InputMap.action_get_events(action):
		if existing is InputEventKey and existing.physical_keycode == physical_key:
			return
	InputMap.action_add_event(action, event)
