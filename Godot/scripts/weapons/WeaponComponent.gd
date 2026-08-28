class_name ALWeaponComponent
extends Node

signal ammo_changed(magazine: int, reserve: int)
signal shot_fired(hit_position: Vector3)
signal reload_started
signal reload_finished
signal dry_fired

@export var data_path := "res://data/weapon_catalog.json"

var camera: Camera3D
var weapon_data := {
	"damage": 28.0,
	"roundsPerMinute": 650.0,
	"magazineSize": 30,
	"reserveAmmo": 120,
	"reloadTime": 1.8,
	"range": 150.0,
	"hipFireSpread": 1.8,
	"adsSpread": 0.55,
	"defaultFOV": 90.0,
	"adsFOV": 70.0,
}
var magazine_ammo := 30
var reserve_ammo := 120
var trigger_pressed := false
var is_aiming := false
var is_reloading := false
var cooldown := 0.0
var reload_remaining := 0.0

func _ready() -> void:
	_load_weapon_data()
	magazine_ammo = int(weapon_data.get("magazineSize", 30))
	reserve_ammo = int(weapon_data.get("reserveAmmo", 120))
	ammo_changed.emit(magazine_ammo, reserve_ammo)

func set_camera(value: Camera3D) -> void:
	camera = value

func tick(delta: float) -> void:
	cooldown = maxf(0.0, cooldown - delta)
	if is_reloading:
		reload_remaining -= delta
		if reload_remaining <= 0.0:
			_complete_reload()
		return
	if trigger_pressed:
		_try_fire()

func request_reload() -> void:
	if is_reloading or magazine_ammo >= int(weapon_data.get("magazineSize", 30)) or reserve_ammo <= 0:
		return
	is_reloading = true
	trigger_pressed = false
	reload_remaining = float(weapon_data.get("reloadTime", 1.8))
	reload_started.emit()

func _try_fire() -> void:
	if cooldown > 0.0 or camera == null:
		return
	if magazine_ammo <= 0:
		dry_fired.emit()
		return
	magazine_ammo -= 1
	cooldown = 60.0 / maxf(float(weapon_data.get("roundsPerMinute", 650.0)), 1.0)
	var origin := camera.global_position
	var direction := -camera.global_transform.basis.z
	var spread := float(weapon_data.get("adsSpread", 0.55) if is_aiming else weapon_data.get("hipFireSpread", 1.8))
	direction = direction.rotated(camera.global_transform.basis.x, deg_to_rad(randf_range(-spread, spread)))
	direction = direction.rotated(Vector3.UP, deg_to_rad(randf_range(-spread, spread)))
	var destination := origin + direction.normalized() * float(weapon_data.get("range", 150.0))
	var query := PhysicsRayQueryParameters3D.create(origin, destination)
	query.exclude = [get_parent().get_rid()]
	var hit := get_viewport().get_world_3d().direct_space_state.intersect_ray(query)
	if not hit.is_empty():
		destination = hit.position
		var target = hit.collider
		if target != null and target.has_method("receive_damage"):
			target.receive_damage(float(weapon_data.get("damage", 28.0)), get_parent())
	shot_fired.emit(destination)
	ammo_changed.emit(magazine_ammo, reserve_ammo)

func _complete_reload() -> void:
	is_reloading = false
	var capacity := int(weapon_data.get("magazineSize", 30))
	var transfer := mini(capacity - magazine_ammo, reserve_ammo)
	magazine_ammo += transfer
	reserve_ammo -= transfer
	ammo_changed.emit(magazine_ammo, reserve_ammo)
	reload_finished.emit()

func _load_weapon_data() -> void:
	if not FileAccess.file_exists(data_path):
		return
	var file := FileAccess.open(data_path, FileAccess.READ)
	var parsed = JSON.parse_string(file.get_as_text())
	if typeof(parsed) == TYPE_DICTIONARY and parsed.has("weapons") and not parsed.weapons.is_empty():
		weapon_data = parsed.weapons[0]
