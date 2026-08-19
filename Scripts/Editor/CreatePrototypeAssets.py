"""Create ASH LINE Combat Prototype assets inside Unreal Editor Python.

Run this file with Unreal Editor Python, for example:

  UnrealEditor-Cmd ASH_LINE.uproject -run=pythonscript \
    -script=Scripts/Editor/CreatePrototypeAssets.py -unattended -nop4

The script intentionally refuses ordinary Python execution. It creates lightweight
prototype assets only; it does not import Marketplace content or native code.
"""
from __future__ import annotations

try:
    import unreal
except ImportError as exc:  # pragma: no cover - only reached outside Unreal
    raise SystemExit(
        "CreatePrototypeAssets.py must run through Unreal Editor Python; ordinary Python is unsupported."
    ) from exc

MAP_PATH = "/Game/AshLine/Maps/Namar/L_CombatPrototype"
INPUT_ROOT = "/Game/AshLine/Input"
WEAPON_ROOT = "/Game/AshLine/Weapons/Data"
AI_ROOT = "/Game/AshLine/AI/Data"
HEALTH_ROOT = "/Game/AshLine/Data"
UI_ROOT = "/Game/AshLine/UI"


def ensure_directory(path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def load_class(path: str):
    loaded = unreal.load_class(None, path)
    if not loaded:
        unreal.log_error(f"Could not load Unreal class: {path}")
    return loaded


def load_asset(path: str):
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        unreal.log_warning(f"Asset not found: {path}")
    return asset


def set_if_present(asset, property_name: str, value) -> None:
    try:
        asset.set_editor_property(property_name, value)
    except Exception as exc:
        unreal.log_warning(f"Could not set {property_name} on {asset.get_name()}: {exc}")


def create_data_asset(asset_class_path: str, package_path: str, asset_name: str):
    ensure_directory(package_path)
    asset_path = f"{package_path}/{asset_name}"
    existing = load_asset(asset_path)
    if existing:
        return existing
    asset_class = load_class(asset_class_path)
    if not asset_class:
        return None
    factory = unreal.DataAssetFactory()
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    created = asset_tools.create_asset(asset_name, package_path, asset_class, factory)
    if created:
        unreal.EditorAssetLibrary.save_loaded_asset(created)
        unreal.log(f"Created data asset: {asset_path}")
    return created


def create_blueprint(parent_class_path: str, package_path: str, asset_name: str):
    ensure_directory(package_path)
    asset_path = f"{package_path}/{asset_name}"
    existing = load_asset(asset_path)
    if existing:
        return existing
    parent_class = load_class(parent_class_path)
    if not parent_class:
        return None
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    created = asset_tools.create_asset(asset_name, package_path, unreal.Blueprint, factory)
    if created:
        unreal.EditorAssetLibrary.save_loaded_asset(created)
        unreal.log(f"Created Blueprint: {asset_path}")
    return created


def create_widget_blueprint(package_path: str, asset_name: str):
    ensure_directory(package_path)
    asset_path = f"{package_path}/{asset_name}"
    existing = load_asset(asset_path)
    if existing:
        return existing
    factory = unreal.WidgetBlueprintFactory()
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    created = asset_tools.create_asset(asset_name, package_path, unreal.WidgetBlueprint, factory)
    if created:
        unreal.EditorAssetLibrary.save_loaded_asset(created)
        unreal.log(f"Created Widget Blueprint: {asset_path}")
    return created


def create_health_asset():
    health = create_data_asset("/Script/AshLineCombat.ALHealthConfig", HEALTH_ROOT, "DA_Health_100")
    if health:
        set_if_present(health, "MaxHealth", 100.0)
        set_if_present(health, "InitialInvulnerabilityDuration", 0.0)
        unreal.EditorAssetLibrary.save_loaded_asset(health)
    return health


def create_weapon_assets():
    specs = (
        ("DA_WPN_AR_001", "WPN_AR_001", "Assault Rifle", "AssaultRifle", "FullAuto", 28.0, 30, 90, 650.0),
        ("DA_WPN_SMG_001", "WPN_SMG_001", "SMG", "SMG", "FullAuto", 21.0, 32, 96, 850.0),
        ("DA_WPN_PST_001", "WPN_PST_001", "Pistol", "Pistol", "SemiAuto", 32.0, 15, 45, 350.0),
    )
    assets = {}
    for asset_name, weapon_id, display_name, weapon_type, fire_mode, damage, magazine, reserve, rpm in specs:
        asset = create_data_asset("/Script/AshLineWeapons.ALWeaponDataAsset", WEAPON_ROOT, asset_name)
        if not asset:
            continue
        set_if_present(asset, "WeaponId", unreal.Name(weapon_id))
        set_if_present(asset, "DisplayName", unreal.Text(display_name))
        set_if_present(asset, "WeaponType", getattr(unreal.EALWeaponType, weapon_type, weapon_type))
        set_if_present(asset, "FireMode", getattr(unreal.EALFireMode, fire_mode, fire_mode))
        set_if_present(asset, "Damage", damage)
        set_if_present(asset, "MagazineSize", magazine)
        set_if_present(asset, "ReserveAmmo", reserve)
        set_if_present(asset, "RoundsPerMinute", rpm)
        set_if_present(asset, "ReloadTime", 1.2 if weapon_type != "Pistol" else 1.0)
        set_if_present(asset, "Range", 8000.0)
        unreal.EditorAssetLibrary.save_loaded_asset(asset)
        assets[weapon_id] = asset
    return assets


def create_ai_assets(health_asset, weapons):
    specs = (
        ("DA_AI_Soldier_Basic", "AI_Soldier_Basic", 0.55, 0.55, "Basic", "WPN_AR_001"),
        ("DA_AI_Soldier_Trained", "AI_Soldier_Trained", 0.35, 0.72, "Trained", "WPN_AR_001"),
    )
    assets = {}
    for asset_name, archetype_id, reaction, accuracy, tier, weapon_id in specs:
        asset = create_data_asset("/Script/AshLineAI.ALAIArchetypeData", AI_ROOT, asset_name)
        if not asset:
            continue
        set_if_present(asset, "ArchetypeId", unreal.Name(archetype_id))
        set_if_present(asset, "HealthConfig", health_asset)
        if weapons.get(weapon_id):
            set_if_present(asset, "WeaponData", unreal.SoftObjectPath(weapons[weapon_id].get_path_name()))
        set_if_present(asset, "ReactionTime", reaction)
        set_if_present(asset, "Accuracy", accuracy)
        set_if_present(asset, "AccuracyTier", getattr(unreal.EALAIAccuracy, tier, tier))
        set_if_present(asset, "DecisionInterval", 0.35 if tier == "Basic" else 0.3)
        set_if_present(asset, "PerceptionInterval", 0.25)
        set_if_present(asset, "CoverPreference", 0.35 if tier == "Basic" else 0.55)
        unreal.EditorAssetLibrary.save_loaded_asset(asset)
        assets[archetype_id] = asset
    return assets


def create_input_assets():
    ensure_directory(INPUT_ROOT)
    action_class = "/Script/EnhancedInput.InputAction"
    action_specs = {
        "IA_Move": "AXIS2D",
        "IA_Look": "AXIS2D",
        "IA_Jump": "BOOLEAN",
        "IA_Crouch": "BOOLEAN",
        "IA_Sprint": "BOOLEAN",
        "IA_Interact": "BOOLEAN",
        "IA_Fire": "BOOLEAN",
        "IA_Aim": "BOOLEAN",
        "IA_Reload": "BOOLEAN",
        "IA_SwitchWeapon": "BOOLEAN",
        "IA_NextWeapon": "BOOLEAN",
        "IA_Pause": "BOOLEAN",
    }
    actions = {}
    for name, value_type in action_specs.items():
        action = create_data_asset(action_class, INPUT_ROOT, name)
        if action:
            enum_value = getattr(unreal.InputActionValueType, value_type, value_type)
            set_if_present(action, "ValueType", enum_value)
            unreal.EditorAssetLibrary.save_loaded_asset(action)
            actions[name] = action

    context = create_data_asset("/Script/EnhancedInput.InputMappingContext", INPUT_ROOT, "IMC_Player")
    if context:
        # These mappings are the PC baseline. Mobile controls use the same actions
        # through the runtime touch widget/Blueprint layer.
        mapping_specs = {
            "IA_Move": (("W", ("swizzle_y",)), ("S", ("swizzle_y", "negate")), ("D", ()), ("A", ("negate",))),
            "IA_Look": (("MouseX", ()), ("MouseY", ("swizzle_y",))),
            "IA_Jump": (("SpaceBar", ()),),
            "IA_Crouch": (("C", ()),),
            "IA_Sprint": (("LeftShift", ()),),
            "IA_Interact": (("E", ()),),
            "IA_Fire": (("LeftMouseButton", ()),),
            "IA_Aim": (("RightMouseButton", ()),),
            "IA_Reload": (("R", ()),),
            "IA_SwitchWeapon": (("Q", ()),),
            "IA_NextWeapon": (("Q", ()),),
            "IA_Pause": (("Escape", ()),),
        }
        for action_name, mappings in mapping_specs.items():
            action = actions.get(action_name)
            if not action or not hasattr(context, "map_key"):
                continue
            for key_name, modifier_names in mappings:
                try:
                    mapping = context.map_key(action, unreal.Key(key_name))
                    if mapping and hasattr(mapping, "get_editor_property"):
                        modifiers = list(mapping.get_editor_property("modifiers") or [])
                        for modifier_name in modifier_names:
                            if modifier_name == "negate":
                                modifiers.append(unreal.InputModifierNegate())
                            elif modifier_name == "swizzle_y":
                                swizzle = unreal.InputModifierSwizzleAxis()
                                set_if_present(swizzle, "order", getattr(unreal.EInputAxisSwizzle, "YXZ", "YXZ"))
                                modifiers.append(swizzle)
                        mapping.set_editor_property("modifiers", modifiers)
                except Exception as exc:
                    unreal.log_warning(f"Could not map {key_name} to {action_name}: {exc}")
        unreal.EditorAssetLibrary.save_loaded_asset(context)
    return actions, context


def create_blueprints(weapon_assets, input_actions, mapping_context, archetypes):
    weapon_blueprints = {}
    for asset_name, weapon_id in (("BP_WPN_AR_001", "WPN_AR_001"), ("BP_WPN_SMG_001", "WPN_SMG_001"), ("BP_WPN_PST_001", "WPN_PST_001")):
        weapon_bp = create_blueprint("/Script/AshLineWeapons.AALWeaponBase", "/Game/AshLine/Weapons", asset_name)
        weapon_blueprints[weapon_id] = weapon_bp
        if weapon_bp and weapon_assets.get(weapon_id):
            try:
                weapon_bp.generated_class().get_default_object().set_editor_property("Data", weapon_assets[weapon_id])
                unreal.EditorAssetLibrary.save_loaded_asset(weapon_bp)
            except Exception as exc:
                unreal.log_warning(f"Could not assign weapon data to {asset_name}: {exc}")

    player_controller_bp = create_blueprint("/Script/AshLineCharacters.AALPlayerController", "/Game/AshLine/Characters/Player", "BP_ALPlayerController")
    player_bp = create_blueprint("/Script/AshLineCharacters.AALPlayerCharacter", "/Game/AshLine/Characters/Player", "BP_ALPlayerCharacter")
    enemy_bp = create_blueprint("/Script/AshLineAI.AALInfantryCharacter", "/Game/AshLine/AI", "BP_ALInfantry_Test")
    game_mode_bp = create_blueprint("/Script/AshLineMissions.AALCombatPrototypeGameMode", "/Game/AshLine/Core", "BP_ALCombatPrototypeGameMode")
    cover_bp = create_blueprint("/Script/AshLineAI.AALCoverPoint", "/Game/AshLine/AI", "BP_ALCoverPoint")
    patrol_bp = create_blueprint("/Script/AshLineAI.AALPatrolRoute", "/Game/AshLine/AI", "BP_ALPatrolRoute")
    main_menu = create_widget_blueprint(UI_ROOT, "WBP_MainMenu")
    mobile_hud = create_widget_blueprint(UI_ROOT, "WBP_MobileTouchLayer")
    combat_hud = create_widget_blueprint(UI_ROOT, "WBP_CombatPrototypeHUD")
    hud_bp = create_blueprint("/Script/AshLineUI.AALCombatPrototypeHUD", UI_ROOT, "BP_ALCombatPrototypeHUD")

    if player_controller_bp:
        controller_defaults = player_controller_bp.generated_class().get_default_object()
        controller_bindings = {
            "PlayerMappingContext": mapping_context,
            "MoveAction": input_actions.get("IA_Move"),
            "LookAction": input_actions.get("IA_Look"),
            "JumpAction": input_actions.get("IA_Jump"),
            "CrouchAction": input_actions.get("IA_Crouch"),
            "SprintAction": input_actions.get("IA_Sprint"),
            "InteractAction": input_actions.get("IA_Interact"),
            "PauseAction": input_actions.get("IA_Pause"),
            "FireAction": input_actions.get("IA_Fire"),
            "AimAction": input_actions.get("IA_Aim"),
            "ReloadAction": input_actions.get("IA_Reload"),
            "NextWeaponAction": input_actions.get("IA_SwitchWeapon") or input_actions.get("IA_NextWeapon"),
            "MobileTouchWidgetClass": mobile_hud.generated_class() if mobile_hud else None,
        }
        for property_name, value in controller_bindings.items():
            if value:
                set_if_present(controller_defaults, property_name, value)
        unreal.EditorAssetLibrary.save_loaded_asset(player_controller_bp)

    if player_bp:
        player_defaults = player_bp.generated_class().get_default_object()
        loadout_bindings = {
            "PrimaryWeaponClass": weapon_blueprints.get("WPN_AR_001").generated_class() if weapon_blueprints.get("WPN_AR_001") else None,
            "PrimaryWeaponData": weapon_assets.get("WPN_AR_001"),
            "SidearmWeaponClass": weapon_blueprints.get("WPN_PST_001").generated_class() if weapon_blueprints.get("WPN_PST_001") else None,
            "SidearmWeaponData": weapon_assets.get("WPN_PST_001"),
            "DevelopmentSMGClass": weapon_blueprints.get("WPN_SMG_001").generated_class() if weapon_blueprints.get("WPN_SMG_001") else None,
            "DevelopmentSMGData": weapon_assets.get("WPN_SMG_001"),
        }
        for property_name, value in loadout_bindings.items():
            if value:
                set_if_present(player_defaults, property_name, value)
        unreal.EditorAssetLibrary.save_loaded_asset(player_bp)

    if enemy_bp:
        enemy_defaults = enemy_bp.generated_class().get_default_object()
        if weapon_blueprints.get("WPN_AR_001"):
            set_if_present(enemy_defaults, "WeaponClass", weapon_blueprints["WPN_AR_001"].generated_class())
        if archetypes.get("AI_Soldier_Basic"):
            set_if_present(enemy_defaults, "Archetype", archetypes["AI_Soldier_Basic"])
        unreal.EditorAssetLibrary.save_loaded_asset(enemy_bp)

    if hud_bp and combat_hud:
        try:
            hud_bp.generated_class().get_default_object().set_editor_property("CombatWidgetClass", combat_hud.generated_class())
            unreal.EditorAssetLibrary.save_loaded_asset(hud_bp)
        except Exception as exc:
            unreal.log_warning(f"Could not assign CombatWidgetClass on HUD Blueprint: {exc}")
    if game_mode_bp:
        try:
            game_mode_defaults = game_mode_bp.generated_class().get_default_object()
            if player_bp:
                game_mode_defaults.set_editor_property("DefaultPawnClass", player_bp.generated_class())
            if player_controller_bp:
                game_mode_defaults.set_editor_property("PlayerControllerClass", player_controller_bp.generated_class())
            if hud_bp:
                game_mode_defaults.set_editor_property("HUDClass", hud_bp.generated_class())
            game_mode_defaults.set_editor_property("RequiredEnemyCount", 6)
            unreal.EditorAssetLibrary.save_loaded_asset(game_mode_bp)
        except Exception as exc:
            unreal.log_warning(f"Could not assign Combat Prototype GameMode defaults: {exc}")
    return player_bp, player_controller_bp, enemy_bp, game_mode_bp, cover_bp, patrol_bp, main_menu, mobile_hud, combat_hud, hud_bp, weapon_blueprints


def spawn_static_mesh(location, scale, mesh_path="/Engine/BasicShapes/Cube.Cube"):
    actor_class = load_class("/Script/Engine.StaticMeshActor")
    mesh = load_asset(mesh_path)
    if not actor_class or not mesh:
        return None
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(actor_class, unreal.Vector(*location), unreal.Rotator(0, 0, 0))
    if actor:
        actor.static_mesh_component.set_editor_property("static_mesh", mesh)
        actor.set_actor_scale3d(unreal.Vector(*scale))
    return actor


def spawn_actor(class_path, location, rotation=(0, 0, 0)):
    actor_class = load_class(class_path)
    if not actor_class:
        return None
    return unreal.EditorLevelLibrary.spawn_actor_from_class(actor_class, unreal.Vector(*location), unreal.Rotator(*rotation))


def build_arena(player_bp, enemy_bp, cover_bp, patrol_bp, archetypes, weapons):
    if not unreal.EditorLevelLibrary.new_level(MAP_PATH):
        if not unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
            raise RuntimeError(f"Could not create map: {MAP_PATH}")
        unreal.EditorLevelLibrary.load_level(MAP_PATH)

    # Lightweight Arabic urban training blockout: central arena, short street,
    # two buildings, alleys, cover boxes, and a raised wall.
    for location, scale in (
        ((0, 0, -50), (18, 14, 0.5)),
        ((1800, 0, 150), (5, 14, 4)),
        ((-1800, 0, 150), (5, 14, 4)),
        ((0, 1300, 150), (14, 3, 4)),
        ((0, -1300, 150), (14, 3, 4)),
        ((900, 700, 90), (2, 1.5, 1)),
        ((-900, 700, 90), (2, 1.5, 1)),
        ((900, -700, 90), (2, 1.5, 1)),
        ((-900, -700, 90), (2, 1.5, 1)),
        ((0, 900, 220), (8, 0.5, 2.5)),
    ):
        spawn_static_mesh(location, scale)

    spawn_actor("/Script/Engine.PlayerStart", (0, 0, 100))
    nav_volume = spawn_actor("/Script/NavigationSystem.NavMeshBoundsVolume", (0, 0, 100))
    if nav_volume:
        nav_volume.set_actor_scale3d(unreal.Vector(40, 30, 10))

    if patrol_bp:
        spawn_actor(patrol_bp.generated_class().get_path_name(), (-700, 0, 100))
    if cover_bp:
        for location in ((-500, -400, 50), (-500, 400, 50), (0, -550, 50), (0, 550, 50), (500, -400, 50), (500, 400, 50), (1000, 0, 50), (-1000, 0, 50)):
            spawn_actor(cover_bp.generated_class().get_path_name(), location)

    if enemy_bp:
        enemy_class = enemy_bp.generated_class()
        positions = ((850, 350, 100), (850, -350, 100), (0, 1000, 100), (0, -1000, 100), (-850, 350, 100), (-850, -350, 100))
        for index, position in enumerate(positions):
            enemy = spawn_actor(enemy_class.get_path_name(), position)
            if not enemy:
                continue
            archetype = archetypes.get("AI_Soldier_Basic" if index < 4 else "AI_Soldier_Trained")
            if archetype:
                set_if_present(enemy, "Archetype", archetype)
            set_if_present(enemy, "CorpseLifetime", 3.0)

    if player_bp:
        player_class = player_bp.generated_class()
        # The PlayerStart owns the spawn transform; this actor is not placed twice.
        unreal.log(f"Player Blueprint ready for PlayerStart: {player_class.get_path_name()}")

    unreal.EditorLevelLibrary.save_current_level()
    unreal.log(f"Created and saved Combat Prototype map: {MAP_PATH}")


def validate_assets():
    required = (
        MAP_PATH,
        f"{INPUT_ROOT}/IMC_Player",
        f"{INPUT_ROOT}/IA_Move",
        f"{INPUT_ROOT}/IA_Look",
        f"{INPUT_ROOT}/IA_Jump",
        f"{INPUT_ROOT}/IA_Crouch",
        f"{INPUT_ROOT}/IA_Sprint",
        f"{INPUT_ROOT}/IA_Interact",
        f"{INPUT_ROOT}/IA_Fire",
        f"{INPUT_ROOT}/IA_Aim",
        f"{INPUT_ROOT}/IA_Reload",
        f"{INPUT_ROOT}/IA_SwitchWeapon",
        f"{INPUT_ROOT}/IA_NextWeapon",
        f"{INPUT_ROOT}/IA_Pause",
        f"{WEAPON_ROOT}/DA_WPN_AR_001",
        f"{WEAPON_ROOT}/DA_WPN_SMG_001",
        f"{WEAPON_ROOT}/DA_WPN_PST_001",
        f"{AI_ROOT}/DA_AI_Soldier_Basic",
        f"{AI_ROOT}/DA_AI_Soldier_Trained",
        "/Game/AshLine/Characters/Player/BP_ALPlayerCharacter",
        "/Game/AshLine/Characters/Player/BP_ALPlayerController",
        "/Game/AshLine/AI/BP_ALInfantry_Test",
        "/Game/AshLine/Core/BP_ALCombatPrototypeGameMode",
        "/Game/AshLine/Weapons/BP_WPN_AR_001",
        "/Game/AshLine/Weapons/BP_WPN_SMG_001",
        "/Game/AshLine/Weapons/BP_WPN_PST_001",
        f"{UI_ROOT}/BP_ALCombatPrototypeHUD",
        f"{UI_ROOT}/WBP_MainMenu",
        f"{UI_ROOT}/WBP_MobileTouchLayer",
        f"{UI_ROOT}/WBP_CombatPrototypeHUD",
    )
    missing = [path for path in required if not unreal.EditorAssetLibrary.does_asset_exist(path)]
    if missing:
        raise RuntimeError("Prototype asset validation failed: " + ", ".join(missing))
    unreal.log("Combat Prototype asset validation passed.")


def main():
    health = create_health_asset()
    weapons = create_weapon_assets()
    archetypes = create_ai_assets(health, weapons)
    input_actions, mapping_context = create_input_assets()
    player_bp, player_controller_bp, enemy_bp, game_mode_bp, cover_bp, patrol_bp, main_menu, mobile_hud, combat_hud, hud_bp, weapon_blueprints = create_blueprints(weapons, input_actions, mapping_context, archetypes)
    build_arena(player_bp, enemy_bp, cover_bp, patrol_bp, archetypes, weapons)
    validate_assets()


if __name__ == "__main__":
    main()
