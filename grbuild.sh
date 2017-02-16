echo "Removing the app..."
adb shell pm clear art.neural.ovechko.neuralart_android

echo "Building .so lib..."

sh neuralart/build.sh

echo "Gradle..."

./gradlew installDebug

echo "Launch the app..."

adb shell monkey -p art.neural.ovechko.neuralart_android -c android.intent.category.LAUNCHER 1
