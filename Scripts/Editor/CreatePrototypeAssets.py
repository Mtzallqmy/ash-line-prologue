"""Create prototype Unreal assets from inside the Unreal Editor Python environment.

Run from Unreal Editor's Python console or with -ExecutePythonScript. This file is
Editor-only and must not be included in a Shipping package.
"""
from __future__ import annotations

try:
    import unreal
except ImportError as exc:  # pragma: no cover - executed only outside Unreal
    raise SystemExit(
        "CreatePrototypeAssets.py must run through Unreal Editor Python; ordinary Python is unsupported."
    ) from exc

MAP_PATH = "/Game/AshLine/Maps/Namar/L_CombatPrototype"
DATA_ASSETS = (
    ("/Script/AshLineWeapons.ALWeaponDataAsset", "/Game/AshLine/Weapons/Data", "DA_PrototypeWeapon"),
    ("/Script/AshLineMissions.ALMissionDataAsset", "/Game/AshLine/Missions/Data", "DA_PrototypeMission"),
)


def ensure_directory(path: str) -> None:
    unreal.EditorAssetLibrary.make_directory(path)


def create_map() -> None:
    ensure_directory("/Game/AshLine/Maps/Namar")
    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        unreal.log_warning(f"Map already exists: {MAP_PATH}")
        return
    if not unreal.EditorLevelLibrary.new_level(MAP_PATH):
        raise RuntimeError(f"Could not create map: {MAP_PATH}")
    unreal.EditorLevelLibrary.save_current_level()
    unreal.log(f"Created and saved map: {MAP_PATH}")


def create_data_assets() -> None:
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    factory = unreal.DataAssetFactory()
    for class_path, package_path, asset_name in DATA_ASSETS:
        ensure_directory(package_path)
        asset_path = f"{package_path}/{asset_name}"
        if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
            unreal.log_warning(f"Data asset already exists: {asset_path}")
            continue
        asset_class = unreal.load_class(None, class_path)
        if not asset_class:
            unreal.log_error(f"Could not load data asset class: {class_path}")
            continue
        asset = asset_tools.create_asset(asset_name, package_path, asset_class, factory)
        if not asset:
            raise RuntimeError(f"Could not create data asset: {asset_path}")
        unreal.EditorAssetLibrary.save_loaded_asset(asset)
        unreal.log(f"Created and saved data asset: {asset_path}")


def validate_assets() -> None:
    if not unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        raise RuntimeError(f"Required prototype map is missing after creation: {MAP_PATH}")
    for class_path, package_path, asset_name in DATA_ASSETS:
        asset_path = f"{package_path}/{asset_name}"
        if not unreal.EditorAssetLibrary.does_asset_exist(asset_path):
            raise RuntimeError(f"Required data asset is missing after creation: {asset_path}")
    unreal.log("Prototype asset validation passed.")


def main() -> None:
    create_map()
    create_data_assets()
    validate_assets()


if __name__ == "__main__":
    main()
