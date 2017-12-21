# Preview Branch

This preview branch includes open source code and demonstrates new build methodology for ETA Cpp.

Multicast functionality has been separated. It will later be available in the Binary Pack.
For Linux the openssl-devel package is required. To install on Oracle and RedHat Linux: 'yum install openssl-devel.x86_64'

The branch is essentially a copy of the ESDK1.1.1 release with previous build files removed and replaced with CMakeLists.txt files (Cpp).

Disregard for now any build instructions in all readmes under Cpp-C directory.

For both Windows and Linux, you need a version of cmake in your windows environment. The version (`cmake --version`) must be 3.8 or newer.

**To build Cpp/Windows**
1. cd to the Cpp-C directory in the distribution
2. 'mkdir build'
3. 'cd build'  (the name of the build directory does not need to be 'build')
4. run `cmake -G "Visual Studio X Win64"` where X is one of:
	- 14 for Visual Studio 2015
	- 12 for Visual Studio 2013
	- 11 for Visual Studio 2012
	- 10 for Visual Studio 2010
4. open Visual Studio, load Cpp-C/ESDK.sln, and build the solution. The build choices are Debug_MDd and Release_MD; both static and shared versions of the libraries and examples are built for each choice.


**To build Cpp/Linux:**
2. cd to the Cpp-C directory in the distribution
3. decide which version to build. Possibilities are Debug, Optimized. *(note: if you are building a Shared version, you need to cd to Elektron-SDK-BinaryPack/Cpp-C/Eta and run ./LinuxSoLink)*
4. Assuming Debug:
	- `mkdir Debug`
	- `cd Debug`
	- `cmake -DCMAKE_BUILD_TYPE=Debug ..`
	- `make'
	
	
For both Windows and Linux:
	libraries are written to Cpp-C/Ema/Libs and Cpp-C/Eta/Libs
	executables are written to Cpp-C/Ema/Executables and Cpp-C/Eta/Executables


	
# Notes:
- This package contains APIs that are subject to proprietary and open source licenses.
- Please make sure to review the LICENSE.md file.

