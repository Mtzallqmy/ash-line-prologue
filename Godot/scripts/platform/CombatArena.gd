extends Node3D

@onready var player: Node = $Player
@onready var director: Node = $MissionDirector
@onready var hud: Control = $UILayer/CombatHUD
@onready var touch: Control = $UILayer/TouchControls

func _ready() -> void:
	_spawn_arena_cover()
	touch.bind_player(player)
	hud.bind_player(player)
	director.enemy_count_changed.connect(hud.set_enemy_count)
	director.mission_completed.connect(_on_mission_completed)

func _spawn_arena_cover() -> void:
	_spawn_box(Vector3(-5.0, 1.0, 1.0), Vector3(2.0, 2.0, 1.2), Color("19456a"))
	_spawn_box(Vector3(4.5, 0.75, -3.5), Vector3(1.6, 1.5, 1.6), Color("26384a"))
	_spawn_box(Vector3(-2.2, 0.55, -6.5), Vector3(3.4, 1.1, 0.9), Color("273548"))
	_spawn_box(Vector3(7.0, 1.5, 4.0), Vector3(0.9, 3.0, 4.0), Color("1c2b3e"))

func _spawn_box(position_value: Vector3, box_size: Vector3, color: Color) -> void:
	var body := StaticBody3D.new()
	body.position = position_value
	var mesh := MeshInstance3D.new()
	var box_mesh := BoxMesh.new()
	box_mesh.size = box_size
	mesh.mesh = box_mesh
	var material := StandardMaterial3D.new()
	material.albedo_color = color
	material.metallic = 0.18
	material.roughness = 0.82
	mesh.material_override = material
	body.add_child(mesh)
	var collision := CollisionShape3D.new()
	var shape := BoxShape3D.new()
	shape.size = box_size
	collision.shape = shape
	body.add_child(collision)
	add_child(body)

func _on_mission_completed() -> void:
	Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
