extends SceneTree

func _initialize() -> void:
	call_deferred("_run")

func _run() -> void:
	var health := ALHealthComponent.new()
	health.maximum_health = 100.0
	health.current_health = 100.0
	root.add_child(health)
	await process_frame
	if not is_equal_approx(health.apply_damage(40.0), 40.0) or not is_equal_approx(health.current_health, 60.0):
		_fail("Health damage contract failed.")
		return
	if not is_equal_approx(health.heal(15.0), 15.0) or not is_equal_approx(health.current_health, 75.0):
		_fail("Health healing contract failed.")
	health.queue_free()

	var session: Node = root.get_node_or_null("GameSession")
	if session == null:
		_fail("Game session autoload was not initialized.")
		return
	session.set("last_result", "")
	var arena_scene: PackedScene = load("res://scenes/CombatArena.tscn")
	var arena: Node = arena_scene.instantiate()
	root.add_child(arena)
	await process_frame
	await process_frame
	var enemies := get_nodes_in_group("enemy")
	if enemies.size() < 1:
		_fail("Combat arena did not register an enemy.")
		return
	for enemy in enemies:
		enemy.receive_damage(1000.0, null)
	await process_frame
	await process_frame
	if session.get("last_result") != "VICTORY":
		_fail("Mission completion contract failed: %s" % session.get("last_result"))
		return
	print("VERTICAL_SLICE_SMOKE_OK")
	quit(0)

func _fail(message: String) -> void:
	push_error(message)
	quit(1)
