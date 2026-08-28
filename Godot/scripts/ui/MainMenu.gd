extends Control

@onready var start_button: Button = $Center/VBox/StartButton
@onready var quit_button: Button = $Center/VBox/QuitButton
@onready var result_label: Label = $Center/VBox/ResultLabel

func _ready() -> void:
	Input.mouse_mode = Input.MOUSE_MODE_VISIBLE
	start_button.pressed.connect(GameSession.start_mission)
	quit_button.pressed.connect(_quit)
	result_label.text = GameSession.last_result

func _quit() -> void:
	get_tree().quit()
