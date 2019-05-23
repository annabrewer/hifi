Please read the [general build guide](BUILD.md) for information on building other platforms. Only Android specific instructions are found in this file. **Note that these instructions apply to building for Oculus Quest.**

# Dependencies

Building is currently supported on OSX, Windows and Linux, but developers intending to do work on the library dependencies are strongly urged to use 64 bit Linux as a build platform.

### Visual Studio

If you don’t have Community or Professional edition of Visual Studio 2017, download [Visual Studio Community 2017](https://visualstudio.microsoft.com/vs/older-downloads/).

When selecting components, check "Desktop development with C++". Also on the right on the Summary toolbar, check "Windows 8.1 SDK and UCRT SDK" and "VC++ 2015.3 v140 toolset (x86,x64)". If you do not already have a Python development environment installed, also check "Python Development" in this screen.

If you already have Visual Studio installed and need to add Python, open the "Add or remove programs" control panel and find the "Microsoft Visual Studio Installer".  Select it and click "Modify".  In the installer, select "Modify" again, then check "Python Development" and allow the installer to apply the changes.

### Android Studio

Download the [Android Studio](https://developer.android.com/studio/index.html) installer and run it. Once installed, at the welcome screen, click _Configure_ in the lower right corner and select _SDK Manager_.

From the _SDK Platforms_ tab, select API levels 26 and 28.  

From the _SDK Tools_ tab, select the following

* Android SDK Build-Tools
* GPU Debugging Tools
* LLDB 
* Android SDK Platform-Tools
* Android SDK Tools
* NDK (even if you have the NDK installed separately)

Still in the _SDK Tools_ tab, click _Show Package Details_. Select CMake 3.6.4. Do this even if you have a separate CMake installation.

Also, make sure the NDK installed version is 18 (or higher).

# Environment

### Create a keystore in Android Studio
Follow the directions [here](https://developer.android.com/studio/publish/app-signing#generate-key) to create a keystore file. You can save it anywhere (preferably not in the `hifi` folder).

### Set up machine specific Gradle properties

Create a `gradle.properties` file in the `.gradle` folder (`$HOME/.gradle` on Mac, `Users/<yourname>/.gradle` on Windows). Edit the file to contain the following

    HIFI_ANDROID_PRECOMPILED=<your_home_directory>/Android/hifi_externals
    HIFI_ANDROID_KEYSTORE=<key_store_directory>/<keystore_name>.jks
    HIFI_ANDROID_KEYSTORE_PASSWORD=<password>
    HIFI_ANDROID_KEY_ALIAS=<key_alias>
    HIFI_ANDROID_KEY_PASSWORD=<key_passwords>

Note, do not use $HOME for the path. It must be a fully qualified path name. Also, be sure to use forward slashes in your path.

#### If you are building for an Android phone

Add these lines to `gradle.properties`

    SUPPRESS_QUEST_INTERFACE
    SUPPRESS_QUEST_FRAME_PLAYER

#### If you are building for an Oculus Quest

Add these lines to `gradle.properties`

    SUPPRESS_FRAME_PLAYER
    SUPPRESS_INTERFACE

The above code to suppress modules is not necessary, but will speed up the build process.

### Clone the repository

`git clone https://github.com/highfidelity/hifi.git `

# Building & Running

### Building Modules

* Open Android Studio
* Choose _Open an existing Android Studio project_
* Navigate to the `hifi` repository and choose the `android` folder and select _OK_
* Wait for Gradle to sync (this should take around 20 minutes the first time)
* From the _Build_ menu select _Make Project_

### Running a Module

* In the toolbar at the top of Android Studio, next to the green hammer icon, you should see a dropdown menu. From this menu, select _Edit Configurations_
* Click the plus sign in the upper left corner of the new window, and select _Android App_
* From the _Module_ drop down menu, select the module you are intending to run (most likely `interface` or `questInterface`)
* From the _Launch_ drop down menu, select _Specified Activity_
* In the _Activity_ field directly below, put `io.highfidelity.hifiinterface.PermissionChecker`
* Click the green play button in the top toolbar of Android Studio

# Troubleshooting

To view a more complete debug log,

* Click the icon with the two overlapping squares in the upper left corner of the tab where the sync is running (hover text says _Toggle view_)
* To change verbosity, click _File > Settings_. Under _Build, Execution, Deployment > Compiler_ you can add command-line flags, as per Gradle documentation

Some things you can try if you want to do a clean build
 
* Delete the `build` and `.externalNativeBuild` folders from the folder for each module you're building (for example, `hifi/android/apps/interface`)
* If you have set your `HIFI_VCPKG_ROOT` environment variable, delete the contents of that directory; otherwise, delete `AppData/Local/Temp/hifi`
* In Android Studio, click _File > Invalidate Caches / Restart_ and select _Invalidate and Restart_

If you see lots of "couldn't acquire lock" errors,
* Open Task Manager and close any running Clang / Gradle processes