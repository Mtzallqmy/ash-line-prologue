extends CharacterBody3D

signal player_died

@export var walk_speed := 5.2
@export var sprint_speed := 7.2
@export var crouch_speed := 3.1
@export var jump_velocity := 5.8
@export var look_sensitivity := 0.0026

@onready var yaw: Node3D = $Yaw
@onready var pitch: Node3D = $Yaw/Pitch
@onready var camera: Camera3D = $Yaw/Pitch/Camera3D
@onready var health: ALHealthComponent = $Health
@onready var weapon: ALWeaponComponent = $Weapon

var touch_move := Vector2.ZERO
var touch_fire := false
var touch_sprint := false
var touch_aim := false
var can_control := true
var gravity := ProjectSettings.get_setting("physics/3d/default_gravity") as float

func _ready() -> void:
	add_to_group("player")
	weapon.set_camera(camera)
	health.died.connect(_on_health_died)
	Input.mouse_mode = Input.MOUSE_MODE_CAPTURED

func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventMouseMotion and Input.mouse_mode == Input.MOUSE_MODE_CAPTURED:
		add_look_input(event.relative)
	elif event.is_action_pressed("pause_menu"):
		GameSession.return_to_menu()

func _physics_process(delta: float) -> void:
	if not can_control:
		return
	if not is_on_floor():
		velocity.y -= gravity * delta
	if Input.is_action_just_pressed("jump") and is_on_floor():
		velocity.y = jump_velocity
	var movement := Input.get_vector("move_left", "move_right", "move_forward", "move_backward")
	if touch_move.length_squared() > movement.length_squared():
		movement = touch_move
	var target_speed := crouch_speed if Input.is_action_pressed("crouch") else sprint_speed if (Input.is_action_pressed("sprint") or touch_sprint) else walk_speed
	var forward := -yaw.global_transform.basis.z
	forward.y = 0.0
	forward = forward.normalized()
	var right := yaw.global_transform.basis.x.normalized()
	var desired := (right * movement.x + forward * -movement.y) * target_speed
	velocity.x = move_toward(velocity.x, desired.x, target_speed * 10.0 * delta)
	velocity.z = move_toward(velocity.z, desired.z, target_speed * 10.0 * delta)
	move_and_slide()

func _process(delta: float) -> void:
	if not can_control:
		return
	weapon.trigger_pressed = Input.is_action_pressed("fire") or touch_fire
	weapon.is_aiming = Input.is_action_pressed("aim") or touch_aim
	if Input.is_action_just_pressed("reload"):
		weapon.request_reload()
	weapon.tick(delta)
	var target_fov := float(weapon.weapon_data.get("adsFOV", 70.0) if weapon.is_aiming else weapon.weapon_data.get("defaultFOV", 90.0))
	camera.fov = lerpf(camera.fov, target_fov, minf(delta * 12.0, 1.0))

func add_look_input(delta: Vector2) -> void:
	if not can_control:
		return
	yaw.rotate_y(-delta.x * look_sensitivity)
	pitch.rotation.x = clampf(pitch.rotation.x - delta.y * look_sensitivity, deg_to_rad(-85.0), deg_to_rad(85.0))

func set_touch_move(value: Vector2) -> void:
	touch_move = value.limit_length(1.0)

func set_touch_fire(pressed: bool) -> void:
	touch_fire = pressed

func set_touch_sprint(pressed: bool) -> void:
	touch_sprint = pressed

func set_touch_aim(pressed: bool) -> void:
	touch_aim = pressed

func request_reload() -> void:
	weapon.request_reload()

func receive_damage(amount: float, source: Node = null) -> void:
	health.apply_damage(amount, source)

func _on_health_died() -> void:
	if not can_control:
		return
	can_control = false
	velocity = Vector3.ZERO
	weapon.trigger_pressed = false
	player_died.emit()
	GameSession.finish_mission(false)
	get_tree().create_timer(1.5).timeout.connect(GameSession.restart_mission)
