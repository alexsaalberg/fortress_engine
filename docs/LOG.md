# Feat-fetchContent

## FetchContent
So I'm trying to see if I can add Bulet Physics via [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)
I got the idea from [here.](https://www.reddit.com/r/cpp/comments/9284h5/dependency_management_with_cmake_and_git/e33xxh2/)

FetchContent seems to actually download and build the BulletPhysics git repo.
From looking at the CMakeLists.txt in the /examples folder, those examples link the bullet libraries like this
	LINK_LIBRARIES(
		BulletDynamics
	)
They also include the header files via the more 'full' path 
(e.g. `#include BulletCollision/CollisionDispatch/btCollisionObject.h`)
I don't currently do this because I go into XCode's build settings after building and manually set the include to be recursive. (and I include with just `#include "btCollisionObject.h"`)

So hypothetically I could make a /fetch folder in the root, and put a CMakeLists.txt there.
What this would do for each dependency is different, but for example bullet would be:

Have it check for each external dependency to see if it is already installed on the system, if it is include it normally (will still have to change header paths to be more 'full'), if it is not installed do FetchContent for it and then do LINK_LIBRARIES for every library.

Which should work. But I would STILL have the problem of that if anything goes wrong in bullet code it is hard to debug. I DO want to have the full source in my project, and I want it to be debuggable.
MAYBE there is a way to debug with source

## Looking into debugging
Looking into the .dylib files produced by the bullet compilation showed _some_ debug symbols (stuff like `__ZN12b3DynamicBvh7sStkCLN2Ep...`, but it wasn't very clear. Looking at the symbols in the bullet .o files within the .a library files that are installed in /usr/local/lib on my system seemed to indicate those had less symbols. (I used `dsymutil -s <file>` for this)

However, debugging the examples that were compiled has part of bullet using Xcode _does_ allow me to step through the full source code. So I'm guessing that the .dylib files somehow reference the .cpp files for debugging (one stackoverflow answer talking about debug symbols indicated something like this was possible).

SO, I'm gonna go forward with trying to add BulletPhysics to the engine via FetchContent and Link_Libraries and all that and HOPEFULLY that will work fine, not increase the size of the repo (another reason I'd like to not copy the source code directly in), and allow full debugging.

## Looking into adding bullet via FetchContent
So bullet seems to use a bunch of deprecated CMake in their CMakeLists.txt files. (LINK_LIBRARIES, SUB_DIRS, etc.). So it's not as straightforward as simply copying their cmake in.

## Trying more to do fetchContent
I can't really figure it out. I'm trying to pass in CMAKE_ARGS to the bulletPhysics build but it doesn't seem to be working at all.

I think at this point this system won't work, as I clearly don't understand it. So I'm gonna have fetchContent simply copy the bullet physics source files into /ext.

## Trying to include bullet/src manually
So I got it to pretty much work by just downloading bullet via FetchContent and adding it to the project sources (versus compiling it as a library via the bullet CMake files).
There is a small problem where some of the bullet files have unused functions/variables so they show up in the warnings. I've figured out that you can use `set_source_files_properties(<file> PROPERTIES COMPILE_FLAGS "<flags>)` to set flags for individual sources files.
However, according to the CMake documentation for `COMPILE_FLAGS`, "Xcode does not support per-config per-source settings, so expressions that depend on the build configuration are not allowed with that generator.". So this won't work for XCode (what I'm currently using to build).

All things considered though, I think I will go down this route.

**So the new build 'track' for bullet will be **
  1. Use FindBullet.cmake and prioritize on-system bullet if available. 
  2. If needed, fetchContent to download bullet and add sources directly to project. 
  3. Use -wNo-Unused on bullet source files. 
  4. If you're using XCode you'll still have a few unused variable/function warnings from bullet.

Which should all be fine, not require people to manually download bullet, and let people debug through bullet source files if it is downloaded via fetchContent (and not preinstalled).

In the future it might be a good idea to compile Bullet into a library via it's own CMake files, but just compile from /src (not the root dir). Because as of now it says "compiling 200 files" which is a little annoying, and all bullet files are in one huge source_group in the ide.

## Building flags
Building using -WEverything (which on Clang, what I use is apparently ACTUALLY everything) produced hundreds of warnings and errors for IMGUI and GLAD, etc.

Building using -WAll produces the same issues as before. Regardless, it might be a good idea to build libraries seperately.

## Building libraries seperately
So looking into building libraries separately. It seems ideal to have each folder in /ext download and compile one dependency and then use the files to make shared library files usable via `find_library()`

They could also use their default CMakeLists.txt's (in the case of Bullet) and just install them directly on the machine so they could later be used via Find_Package().

I'll look into this more another time.

