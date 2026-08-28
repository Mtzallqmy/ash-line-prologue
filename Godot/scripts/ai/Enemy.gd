extends CharacterBody3D

signal defeated(enemy: Node)

enum State { IDLE, CHASE, ATTACK, DEAD }

@export var movement_speed := 2.8
@export var detection_range := 18.0
@export var attack_range := 2.0
@export var attack_damage := 9.0
@export var attack_interval := 1.0

@onready var health: ALHealthComponent = $Health

var state := State.IDLE
var target: CharacterBody3D
var attack_cooldown := 0.0
var decision_cooldown := 0.0
var gravity := ProjectSettings.get_setting("physics/3d/default_gravity") as float

func _ready() -> void:
	add_to_group("enemy")
	health.died.connect(_on_health_died)
	call_deferred("_find_player")

func _physics_process(delta: float) -> void:
	if state == State.DEAD:
		return
	if not is_on_floor():
		velocity.y -= gravity * delta
	attack_cooldown = maxf(0.0, attack_cooldown - delta)
	decision_cooldown -= delta
	if decision_cooldown <= 0.0:
		decision_cooldown = 0.12
		_update_state()
	if target != null and state == State.CHASE:
		var planar := target.global_position - global_position
		planar.y = 0.0
		if planar.length() > 0.15:
			velocity.x = planar.normalized().x * movement_speed
			velocity.z = planar.normalized().z * movement_speed
			look_at(global_position + planar, Vector3.UP, true)
	elif state == State.ATTACK:
		velocity.x = 0.0
		velocity.z = 0.0
		if attack_cooldown <= 0.0 and target != null:
			attack_cooldown = attack_interval
			target.receive_damage(attack_damage, self)
	else:
		velocity.x = 0.0
		velocity.z = 0.0
	move_and_slide()

func receive_damage(amount: float, source: Node = null) -> void:
	health.apply_damage(amount, source)

func _find_player() -> void:
	target = get_tree().get_first_node_in_group("player") as CharacterBody3D

func _update_state() -> void:
	if target == null or not is_instance_valid(target):
		_find_player()
	if target == null:
		state = State.IDLE
		return
	var distance := global_position.distance_to(target.global_position)
	if distance <= attack_range:
		state = State.ATTACK
	elif distance <= detection_range:
		state = State.CHASE
	else:
		state = State.IDLE

func _on_health_died() -> void:
	if state == State.DEAD:
		return
	state = State.DEAD
	defeated.emit(self)
	queue_free()
