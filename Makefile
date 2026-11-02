.PHONY: desktop apk release clean

desktop:
	cmake -S . -B build/desktop
	cmake --build build/desktop -j

runDesktop: buildDekstop
	./bin/desktop/desktop

apk:
	./gradlew assembleDebug
	mkdir -p bin/android
	cp app/build/outputs/apk/debug/app-debug.apk bin/android/

# TODO: release is only "signed" because build.gradle uses the debug key
# (signingConfig signingConfigs.debug). Quick hack, not for the Play Store.
# Set up a real keystore + signingConfig before publishing.
release:
	./gradlew assembleRelease
	mkdir -p bin/android
	cp app/build/outputs/apk/release/app-release.apk bin/android/

clean:
	rm -rf build bin
	./gradlew clean
