extends Control

var player: Node
var move_touch_id := -1
var look_touch_id := -1
var move_origin := Vector2.ZERO
var joystick_vector := Vector2.ZERO

func _ready() -> void:
	set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	mouse_filter = Control.MOUSE_FILTER_STOP
	_add_action_button("FIRE", Vector2(-148, -156), _set_fire.bind(true), _set_fire.bind(false))
	_add_action_button("RELOAD", Vector2(-256, -82), _reload, Callable())
	_add_action_button("SPRINT", Vector2(-148, -82), _set_sprint.bind(true), _set_sprint.bind(false))
	_add_action_button("AIM", Vector2(-256, -156), _set_aim.bind(true), _set_aim.bind(false))

func bind_player(value: Node) -> void:
	player = value

func _gui_input(event: InputEvent) -> void:
	if player == null:
		return
	if event is InputEventMouseMotion and Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
		player.add_look_input(event.relative)
	elif event is InputEventScreenTouch:
		if event.pressed:
			if event.position.x < size.x * 0.46 and move_touch_id < 0:
				move_touch_id = event.index
				move_origin = event.position
				joystick_vector = Vector2.ZERO
			else:
				look_touch_id = event.index
		else:
			if event.index == move_touch_id:
				move_touch_id = -1
				joystick_vector = Vector2.ZERO
				player.set_touch_move(Vector2.ZERO)
			if event.index == look_touch_id:
				look_touch_id = -1
	elif event is InputEventScreenDrag:
		if event.index == move_touch_id:
			joystick_vector = (event.position - move_origin).limit_length(96.0) / 96.0
			player.set_touch_move(joystick_vector)
			queue_redraw()
		elif event.index == look_touch_id:
			player.add_look_input(event.relative * 0.78)

func _add_action_button(label_text: String, offset: Vector2, pressed: Callable, released: Callable) -> void:
	var button := Button.new()
	button.text = label_text
	button.custom_minimum_size = Vector2(96, 56)
	button.set_anchors_preset(Control.PRESET_BOTTOM_RIGHT)
	button.position = offset
	button.add_theme_font_size_override("font_size", 15)
	button.modulate = Color(0.75, 0.9, 1.0, 0.92)
	if pressed.is_valid():
		button.button_down.connect(pressed)
	if released.is_valid():
		button.button_up.connect(released)
	add_child(button)

func _set_fire(pressed: bool) -> void:
	if player != null:
		player.set_touch_fire(pressed)

func _set_sprint(pressed: bool) -> void:
	if player != null:
		player.set_touch_sprint(pressed)

func _set_aim(pressed: bool) -> void:
	if player != null:
		player.set_touch_aim(pressed)

func _reload() -> void:
	if player != null:
		player.request_reload()

func _draw() -> void:
	if move_touch_id < 0:
		return
	draw_circle(move_origin, 56.0, Color(0.25, 0.55, 0.78, 0.20))
	draw_circle(move_origin + joystick_vector * 56.0, 24.0, Color(0.75, 0.9, 1.0, 0.55))
