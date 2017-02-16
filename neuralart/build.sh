#ndk build

ndk-build NDK_TOOLCHAIN_VERSION=4.9  NDK_PROJECT_PATH=neuralart/src/main

mkdir -p neuralart/src/main/libs/armeabi-v7a
cp -r neuralart/src/main/jni/prebuilts/ neuralart/src/main/libs/armeabi-v7a/