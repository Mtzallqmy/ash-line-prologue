extends Node

signal enemy_count_changed(defeated: int, remaining: int)
signal mission_completed

@export var required_enemy_count := 3

var defeated_count := 0
var completed := false
var registered: Array[Node] = []

func _ready() -> void:
	call_deferred("_register_existing_enemies")

func _register_existing_enemies() -> void:
	var enemies := get_tree().get_nodes_in_group("enemy")
	if enemies.size() > 0:
		required_enemy_count = enemies.size()
	for enemy in enemies:
		register_enemy(enemy)
	enemy_count_changed.emit(defeated_count, remaining_count())

func register_enemy(enemy: Node) -> void:
	if enemy == null or registered.has(enemy):
		return
	registered.append(enemy)
	if enemy.has_signal("defeated"):
		enemy.defeated.connect(_on_enemy_defeated)

func remaining_count() -> int:
	return maxi(0, required_enemy_count - defeated_count)

func _on_enemy_defeated(_enemy: Node) -> void:
	if completed:
		return
	defeated_count += 1
	enemy_count_changed.emit(defeated_count, remaining_count())
	if defeated_count >= required_enemy_count:
		completed = true
		mission_completed.emit()
		GameSession.finish_mission(true)
