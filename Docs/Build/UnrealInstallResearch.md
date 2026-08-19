# Unreal Engine Installation Feasibility Notes

## Verified official requirements

Epic's official documentation states that access to the Unreal Engine source repository requires an Epic Games account, a GitHub account, and linking the two accounts through the Epic account dashboard before the `EpicGames/UnrealEngine` repository can be downloaded. The source repository is governed by the Unreal Engine EULA. The alternative is installing a binary engine through Epic's official download/launcher path.

For Linux development, Epic documents Ubuntu 22.04 or Rocky Linux 8 as supported development environments, with a native toolchain installed by `Setup.sh`. The current Sandbox is Ubuntu 24.04 and does not contain the engine, the source repository, an Epic login session, or an authenticated Epic GitHub entitlement. A source build would also require downloading a very large engine tree and compiling the editor/toolchain; the current project specifically targets Unreal Engine 5.4 and Android packaging.

## Local checks

The following paths were searched and did not contain Unreal binaries:

```text
/opt/UnrealEngine
/home/ubuntu/UnrealEngine
/home/ubuntu/EpicGames
```

A broader local filesystem search for `UnrealEditor`, `UnrealEditor-Cmd`, `UnrealBuildTool`, and `UnrealBuildTool.exe` returned no matches. There is no `UE_ROOT` and no UnrealBuildTool executable.

## Android toolchain now installed

The following components were installed and verified independently of Unreal:

```text
Android SDK: /usr/lib/android-sdk
Android Platform: /usr/lib/android-sdk/platforms/android-34
Build Tools: /usr/lib/android-sdk/build-tools/34.0.0
Android NDK: /usr/lib/android-sdk/ndk/25.2.9519653
ADB: /usr/bin/adb, version 34.0.4
Java: OpenJDK 21.0.11
```

`adb devices` runs successfully but no device is connected.

## Conclusion

Android prerequisites can be installed automatically in this Sandbox. Unreal Engine cannot be installed through the available Ubuntu package sources, and its official source/binary access requires authenticated Epic/GitHub entitlement or a pre-provisioned engine archive. Without UnrealEditor/UBT/UHT, it is impossible to execute the requested Compile/Cook/Package steps or generate a legitimate APK. No engine binary or APK should be fabricated.

## References

[1]: https://dev.epicgames.com/documentation/unreal-engine/downloading-source-code-in-unreal-engine "Epic Games — Downloading Unreal Engine Source Code from GitHub"
[2]: https://dev.epicgames.com/documentation/unreal-engine/linux-development-requirements-for-unreal-engine "Epic Games — Linux Development Requirements for Unreal Engine"
[3]: https://www.unrealengine.com/ue-on-github "Epic Games — Accessing Unreal Engine Source Code on GitHub"
