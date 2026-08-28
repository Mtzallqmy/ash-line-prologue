class_name ALHealthComponent
extends Node

signal health_changed(previous: float, current: float, maximum: float, delta: float)
signal damage_received(amount: float, source: Node)
signal died

@export var maximum_health := 100.0
@export var spawn_protection_seconds := 0.0

var current_health := 100.0
var is_dead := false
var is_invulnerable := false

func _ready() -> void:
	current_health = clampf(current_health, 0.0, maximum_health)
	if current_health <= 0.0 and maximum_health > 0.0:
		current_health = maximum_health
	if spawn_protection_seconds > 0.0:
		is_invulnerable = true
		get_tree().create_timer(spawn_protection_seconds).timeout.connect(_clear_spawn_protection)

func apply_damage(amount: float, source: Node = null) -> float:
	if is_dead or is_invulnerable or amount <= 0.0 or maximum_health <= 0.0:
		return 0.0
	var previous := current_health
	current_health = clampf(current_health - amount, 0.0, maximum_health)
	var applied := previous - current_health
	if applied <= 0.0:
		return 0.0
	health_changed.emit(previous, current_health, maximum_health, -applied)
	damage_received.emit(applied, source)
	if current_health <= 0.0:
		is_dead = true
		died.emit()
	return applied

func heal(amount: float) -> float:
	if is_dead or amount <= 0.0:
		return 0.0
	var previous := current_health
	current_health = clampf(current_health + amount, 0.0, maximum_health)
	var applied := current_health - previous
	if applied > 0.0:
		health_changed.emit(previous, current_health, maximum_health, applied)
	return applied

func reset_health() -> void:
	var previous := current_health
	current_health = maximum_health
	is_dead = false
	is_invulnerable = false
	if not is_equal_approx(previous, current_health):
		health_changed.emit(previous, current_health, maximum_health, current_health - previous)

func percentage() -> float:
	return current_health / maximum_health if maximum_health > 0.0 else 0.0

func _clear_spawn_protection() -> void:
	is_invulnerable = false
