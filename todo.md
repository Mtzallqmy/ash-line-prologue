# Project TODO

- [x] Audit Unreal modules, configuration, scripts, and transferable source assets for the Godot migration.
- [x] Create migration documentation mapping Unreal gameplay systems and unavailable assets to Godot replacements.
- [x] Scaffold a modular Godot 4 GDScript project without modifying or depending on the Unreal runtime.
- [x] Implement the mobile-first 3D vertical slice: menu, player, touch input, camera, enemy AI, weapon, health, win/lose, and HUD.
- [ ] Adapt the minimum Android toolchain and one-command build flow for Godot without Unreal, Android Studio, or Visual Studio.
- [ ] Export, inspect, and test an ARM64 Android APK for Android 8 and later.
- [ ] Add Godot-oriented CI, migration status, and commit the migration source without generated binaries or credentials.
- [x] Add a build-progress status file reporting current download percentage, completed bytes, remaining bytes, and the active toolchain phase.
- [ ] Add the minimal Godot Gradle build template required to enforce Android minSdk 26, then rebuild and revalidate the APK.
- [ ] Commit and push the Godot migration source and build documentation to the dedicated GitHub branch.
- [ ] Create the first GitHub release with the verified Android APK and its SHA-256 checksum attachment.
- [ ] Extend the temporary dashboard to show the overall remaining release workflow and a percentage for each active or pending operation.
- [ ] Publish the first GitHub release before attempting the optional physical Android device install test.
- [ ] Provide post-release installation, launch, and one-command build instructions in Arabic.
- [ ] Attach the verified Development APK to this conversation, clearly marked as a pre-release build.
