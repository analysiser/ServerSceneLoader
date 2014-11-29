To Build:
1. In root directory, mkdir build, cd into build
2. On Mac OSX, use: cmake -G 'Xcode' ../src
3. Get into the xcode project file, make build inside
4. Bin file would be in build/sceneloader/Debug/

Build file includes tbb libs. Even not used, it might have build errors.
If this happens, go to src/build folder, open CMakeList.txt file, find:
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	set(TBB_DEBUG "/usr/local/Cellar/tbb/4.3-20140724/lib/libtbb.dylib")
	set(TBB_RELEASE "/usr/local/Cellar/tbb/4.3-20140724/lib/libtbb.dylib")

using Clang, you probably need to specify the actual tbb lib on your local machine. If you are using HomeBrew, probably it is in the same directory.
