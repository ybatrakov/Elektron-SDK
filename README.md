This preview branch demonstrates new build methodology for both Cpp and Java.

The branch is essentially a copy of the ESDK1.1.1 release with previous build files removed and replaced with CMakeLists.txt files (Cpp) and build.gradle files (Java).

**To build Cpp/Windows**

1. you need a version of cmake in your windows environment. The version (`cmake --version`) must be 3.8 or newer.
2. cd to the Cpp-C directory in the distribution
3. run `cmake -G "Visual Studio X Win64"` where X is one of:
	- 14 2015
	- 12 2013
	- 11 2012
	- 10 2010
4. open Visual Studio and load Cpp-C/ESDK.sln. The build choices are Debug_MDd and Release_MD; both static and shared versions of the libraries and examples are built for each choice.

**To build Cpp/Linux:**

1. you need a version of cmake in your Linux environment.  The version (cmake --version) must be 3.8 or newer.
2. cd to the Cpp-C directory in the distribution
3. decide which version to build. Possibilities are DebugStatic, DebugShared, OptimizedStatic, OptimizedShared. *(note: if you are building a Shared version, you need to cd to Elektron-SDK-BinaryPack/Cpp-C/Eta and run ./LinuxSoLink)*
4. Assuming DebugStatic:
	- `mkdir DebugStatic`
	- `cd DebugStatic`
	- `cmake -DCMAKE_BUILD_TYPE=DebugStatic .. > cmake.out` (exit code 0 indicates success)
	- `make > make.out 2>&1` (exit code 0 indicates success)

For both Windows and Linux, libraries and examples are written to
	
- Eta/Libs
- Eta/executables
- Ema/Libs
- Ema/executables

**To open Elektron-SDK projects in Eclipse (tested with Eclipse Oxygen)**

1. In Eclipse go to File->Import
2. Select Gradle->Existing Gradle Project
3. Point the Project root directory to Java directory in the distribution
4. Hit Finish
5. If you see any build errors in the Eclipse you will have to change the Restricted API access rules from error to warnings in the Java/Compiler/Error/Warnings workspace setup

**To build Java Elektron-SDK libraries with gradle on windows.**

1. Open Command Line Window.
2. cd to the Java directory in the distribution
3. run `gradlew.bat assemble`
4. to see available tasks/targets run `gradlew.bat tasks --all`

**To build Java Elektron-SDK libraries with gradle on Unix.** *(To configure proxy follow instructions here: [https://docs.gradle.org/current/userguide/build_environment.html](https://docs.gradle.org/current/userguide/build_environment.html))*

1. cd to the Java directory in the distribution
2. run `./gradlew assemble`
3. to see available tasks/targets run `./gradlew tasks --all`

