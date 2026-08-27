const projectId = process.env.EXPO_PROJECT_ID || undefined;

export default {
  expo: {
    name: 'ASH LINE — Prologue',
    slug: 'ash-line-prologue',
    version: '0.0.1',
    orientation: 'landscape',
    userInterfaceStyle: 'dark',
    platforms: ['android', 'web'],
    android: {
      package: 'com.ashline.game',
      versionCode: 1,
      adaptiveIcon: {
        backgroundColor: '#111827'
      },
      permissions: []
    },
    web: {
      bundler: 'metro'
    },
    plugins: [
      [
        'expo-build-properties',
        {
          android: {
            minSdkVersion: 26,
            compileSdkVersion: 36,
            targetSdkVersion: 36,
            buildToolsVersion: '36.0.0',
            buildArchs: ['arm64-v8a'],
            enableMinifyInReleaseBuilds: true,
            enableShrinkResourcesInReleaseBuilds: true,
            enableBundleCompression: true
          }
        }
      ]
    ],
    extra: {
      eas: projectId ? { projectId } : undefined,
      build: {
        channel: process.env.BUILD_CHANNEL || 'local'
      }
    }
  }
};
