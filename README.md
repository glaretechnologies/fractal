# Fractal

A realtime fractal rendering program.  Uses OpenGL to render on your GPU.

It also serves as a simple introduction to getting an OpenGL shader up and running, and displayed fullscreen on your monitor with C++.

<a href="http://www.youtube.com/watch?feature=player_embedded&v=JUQ6srhYKMQ
" target="_blank"><img src="http://img.youtube.com/vi/JUQ6srhYKMQ/0.jpg" 
alt="IMAGE ALT TEXT HERE" width="240" height="180" border="10" /></a>


# Supported platforms

Should work on Windows, Mac and Linux.  Only tested on Windows so far though!

# How to build

## Prerequisites

* CMake - not strictly needed, but there is a CMake project that you can use.

* SDL - You will need to download and build SDL, or download pre-build SDL binaries.  Get it from here: https://www.libsdl.org/

# Building Fractal

* Download / check out the source

* Make a directory to build in.  Open a powershell window and change to that directory
```
cd D:\programming\fractal\fractal_build
```
* Run cmake to build the compiler project.
```
cmake ..\trunk\trunk\ -G "Visual Studio 14 2015 Win64" -DSDL_DIR=D:\programming
\SDL2-2.0.9 -DSDL_BUILD_DIR=D:\programming\SDL2-2.0.9\VisualC\x64\Debug
```
The first argument to cmake, `..\trunk\trunk\`, is the directory where you checked out the fractal source.

`-G "Visual Studio 14 2015 Win64"` selects Visual Studio 2015 as the compiler, and selects a 64 bit build.

`-DSDL_DIR=D:\programming\SDL2-2.0.9` sets the directory of the SDL source on disk.  This dir is used to find the SDL include files.

`-DSDL_BUILD_DIR=D:\programming\SDL2-2.0.9\VisualC\x64\Debug` is the SDL build dir.  I have used the VS project files included in SDL, and that's where they output the built .libs and .dlls.

* Open the generated compiler project, and build the project.
On Windows:
```
.\Fractal.sln
```
to open, Then press F7 to build.

* Copy SDL2.dll from your SDL build dir to your fractal build dir, e.g. from `D:\programming\SDL2-2.0.9\VisualC\x64\Debug`  to `D:\programming\fractal\fractal_build`.

* Back in Visual Studio, run the 'fractal' project.
