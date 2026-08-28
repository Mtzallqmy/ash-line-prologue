extends Control

var player: Node
var health_label: Label
var ammo_label: Label
var objective_label: Label
var status_label: Label

func _ready() -> void:
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	queue_redraw()
	health_label = _make_label(Vector2(28, 22), 26)
	ammo_label = _make_label(Vector2(28, 58), 22)
	objective_label = _make_label(Vector2(28, 94), 20)
	status_label = _make_label(Vector2(0, 180), 36)
	status_label.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	status_label.set_anchors_preset(Control.PRESET_CENTER_TOP)
	status_label.offset_left = -280.0
	status_label.offset_right = 280.0
	status_label.text = ""
	GameSession.mission_finished.connect(_on_mission_finished)

func bind_player(value: Node) -> void:
	player = value
	player.health.health_changed.connect(_on_health_changed)
	player.weapon.ammo_changed.connect(_on_ammo_changed)
	_on_health_changed(player.health.current_health, player.health.current_health, player.health.maximum_health, 0.0)
	_on_ammo_changed(player.weapon.magazine_ammo, player.weapon.reserve_ammo)

func set_enemy_count(defeated: int, remaining: int) -> void:
	objective_label.text = "OBJECTIVE  •  CLEAR HOSTILES  %d REMAINING" % remaining

func _on_health_changed(_previous: float, current: float, maximum: float, _delta: float) -> void:
	health_label.text = "HEALTH  %03d / %03d" % [roundi(current), roundi(maximum)]

func _on_ammo_changed(magazine: int, reserve: int) -> void:
	ammo_label.text = "AMMO  %02d / %03d" % [magazine, reserve]

func _on_mission_finished(victory: bool) -> void:
	status_label.text = "MISSION COMPLETE" if victory else "MISSION FAILED"

func _make_label(position_value: Vector2, font_size: int) -> Label:
	var label := Label.new()
	label.position = position_value
	label.add_theme_font_size_override("font_size", font_size)
	label.add_theme_color_override("font_color", Color("d9ecff"))
	label.add_theme_color_override("font_shadow_color", Color(0.0, 0.0, 0.0, 0.9))
	label.add_theme_constant_override("shadow_offset_x", 2)
	label.add_theme_constant_override("shadow_offset_y", 2)
	add_child(label)
	return label

func _draw() -> void:
	var center := size * 0.5
	var color := Color("d9ecff")
	draw_line(center + Vector2(-14, 0), center + Vector2(-4, 0), color, 2.0)
	draw_line(center + Vector2(4, 0), center + Vector2(14, 0), color, 2.0)
	draw_line(center + Vector2(0, -14), center + Vector2(0, -4), color, 2.0)
	draw_line(center + Vector2(0, 4), center + Vector2(0, 14), color, 2.0)
