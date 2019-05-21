Please read the [general build guide](BUILD.md) for information on building other platform. Only Android specific instructions are found in this file.

# Dependencies

Building is currently supported on OSX, Windows and Linux platforms, but developers intending to do work on the library dependencies are strongly urged to use 64 bit Linux as a build platform

You will need the following tools to build Android targets.

* [Android Studio](https://developer.android.com/studio/index.html)

### Android Studio

Download the Android Studio installer and run it.   Once installed, at the welcome screen, click _Configure_ in the lower right corner and select _SDK Manager_

From the SDK Platforms tab, select API levels 26 and 28.  

From the SDK Tools tab select the following

* Android SDK Build-Tools
* GPU Debugging Tools
* CMake (even if you have a separate CMake installation)
* LLDB 
* Android SDK Platform-Tools
* Android SDK Tools
* NDK (even if you have the NDK installed separately)

Make sure the NDK installed version is 18 (or higher)

# Environment

Clone the repository

`git clone https://github.com/highfidelity/hifi.git`

# Building & Running

* Open Android Studio
* Choose _Open Existing Android Studio Project_
* Navigate to the `hifi` repository and choose the `android` folder and select _OK_
* Wait for Gradle to sync (this should take around 20 minutes)
* From the _Build_ menu select _Make Project_
* Once the build completes, from the _Run_ menu select _Run App_ 

