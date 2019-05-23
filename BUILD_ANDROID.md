Please read the [general build guide](BUILD.md) for information on building other platform. Only Android specific instructions are found in this file.

# Dependencies

Building is currently supported on OSX, Windows and Linux platforms, but developers intending to do work on the library dependencies are strongly urged to use 64 bit Linux as a build platform

You will need the following tools to build Android targets.

* [Android Studio](https://developer.android.com/studio/index.html)

### Visual Studio

If you don’t have Community or Professional edition of Visual Studio 2017, download [Visual Studio Community 2017](https://www.visualstudio.com/downloads/).

When selecting components, check "Desktop development with C++". Also on the right on the Summary toolbar, check "Windows 8.1 SDK and UCRT SDK" and "VC++ 2015.3 v140 toolset (x86,x64)". If you do not already have a python development environment installed, also check "Python Development" in this screen.

If you already have Visual Studio installed and need to add python, open the "Add or remove programs" control panel and find the "Microsoft Visual Studio Installer".  Select it and click "Modify".  In the installer, select "Modify" again, then check "Python Development" and allow the installer to apply the changes.

### Android Studio

Download the * [Android Studio](https://developer.android.com/studio/index.html) installer and run it. Once installed, at the welcome screen, click _Configure_ in the lower right corner and select _SDK Manager_

From the _SDK Platforms_ tab, select API levels 26 and 28.  

In the _SDK Tools_ tab, select the following

* Android SDK Build-Tools
* GPU Debugging Tools
* LLDB 
* Android SDK Platform-Tools
* Android SDK Tools
* NDK (even if you have the NDK installed separately)

Still in the _SDK Tools_ tab, click _Show Package Details_. Select CMake 3.6.4. Do this even if you have a separate CMake installation.

Also, make sure the NDK installed version is 18 (or higher)

# Environment

### Set up machine specific Gradle properties

Follow the directions [here](https://developer.android.com/studio/publish/app-signing#generate-key) to create a keystore file. You can save it anywhere (preferably not in the `hifi` folder).

Create a `gradle.properties` file in the `.gradle` folder (`$HOME/.gradle` on Mac, `Users/<yourname>/.gradle` on Windows). Edit the file to contain the following

`
HIFI_ANDROID_PRECOMPILED=<your_home_directory>/Android/hifi_externals
HIFI_ANDROID_KEYSTORE=<key_store_directory>/<keystore_name>.jks
HIFI_ANDROID_KEYSTORE_PASSWORD=<password>
HIFI_ANDROID_KEY_ALIAS=<key_alias>
HIFI_ANDROID_KEY_PASSWORD=<key_password>

#SUPPRESS_QUEST_INTERFACE
#SUPPRESS_FRAME_PLAYER
#SUPPRESS_QUEST_FRAME_PLAYER
#SUPPRESS_INTERFACE
`

Uncomment any of the last four lines to avoid building a specific module.

Note, do not use $HOME for the path. It must be a fully qualified path name.

### Set up the repository

Clone the repository

`git clone https://github.com/highfidelity/hifi.git `

# Building & Running

* Open Android Studio
* Choose _Open Existing Android Studio Project_
* Navigate to the `hifi` repository and choose the `android` folder and select _OK_
* Wait for Gradle to sync (this should take around 20 minutes)
* From the _Build_ menu select _Make Project_
* Once the build completes, from the _Run_ menu select _Run App_ 

#Troubleshooting

To view a more complete debug log,

* Click the icon with the two overlapping squares in the upper left corner of the tab where the sync is running (hover text says _Toggle view_).
* To change verbosity, click _File > Settings_. Under _Build, Execution, Deployment > Compiler_ you can add command-line flags, as per Gradle documentation.

If you've made some changes and want to do a clean build,
 
* Delete the `build` and `.externalNativeBuild` folders from the folder for each module you're building (for example, `hifi/android/apps/interface`).
* In Android Studio, click _File > Invalidate Caches / Restart_ and select _Invalidate and Restart_.

If the Gradle sync fails due to a "missing vcpkg configuration" error from `hifi/CMakeLists.txt`,

* If you have set your `HIFI_VCPKG_ROOT` environment variable, delete the contents of that directory.
* Otherwise, delete `AppData/Local/Temp/hifi`.
* Follow the directions above to do a clean build.
