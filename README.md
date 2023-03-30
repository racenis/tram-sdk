# Tramway Drifting and Dungeon Exploration Simulator 2022 Software Development Kit

![Tramway Drifting and Dungeon Exploration Simulator 2022 logo](/docs/logo.gif)

![Software Development Kit logo](/docs/devkit.gif)

## What's it

Software library and tools for making the Tramway Drifting and Dungeon Exploration Simulator 2022.

![Software Development Kit logo](/docs/screen11.png)

### How finished is it?

It's usable, but the programming interfaces aren't very stable. User experience also is very lacking, but it's getting better. Need to write up the documentation.

### Links

More information and also screenshots can be found on the [github.io page](https://racenis.github.io/tram-sdk/). 

Instruction manual and other documentations [can be found here](https://racenis.github.io/tram-sdk/manual/). 

### Dependencies
Right now I'm using glad for OpenGL pointer acquisition, glfw for windowing, glm for vector math, Bullet for physics simulation. Maybe will change in the future.

### Building
I don't have any makefiles right now, so I suggest that you dump all of the code from the `src` directory into your favorite IDE.

Set the compiler include search directories to `src` and `libraries`.

For the linker, set the search directories to `libraries/binaries/win64`, if you're compiling on windows. For Linux you should install the relevant packages from your package manager. Here's the linking order that should work: `-Llibraries  -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -lglfw3 -lOpenAL32  -lglfw3 -lgdi32 -lopengl32`. The last two are for windows only, I think.

### Libraries
For convenience, I have included all of the library header files in the `libraries` directory. Since compiling all of the libraries from source takes about an hour, I have also included binaries for 64-bit windows in the `libraries/binaries/win64` directory. They should work for gcc and clang compilers.

If you're on Linux, then all of the binaries should be available to you from your package manager.

If you're using the microsoft's C++ compiler, then good luck.

### Contributions
No code contributions accepted right now, but if you find a bug or have a suggestion, then you can open a github issue.

## To-do list

#### Should do sooner
- Keybinding menu
- Graphics menu

- ~~Make isloaded in Entity to is_loaded~~
- ~~Yeet material types and vertex types from material and vertex classes resplectivley~~
- ~~Make single loading function load only single resource~~
- Rewrite file parser for the third time
- ~~Add some sort of a memory pool for Messages and Events~~
- ~~Change UpdateLocation/Rotation to SetLocation/Rotation in RenderComponent~~
- ~~Make UID == operator to work with const char*~~
- ~~Add IDs to entity definitions in worldcell files~~
- ~~Allow entities without names~~
- ~~Allow enitties without IDs~~
	- ~~In that case the IDs would be generated automatically~~
- ~~Make Entity FindByName and FindByID overloads of Find~~
- Make the engine work without any files
	- That means that it will run just fine without any files on the disk
	- ~~Default/error models/materials should be baked into source code~~
		- ~~Error model~~
		- ~~Error texture~~
		- ~~Fullbright texture~~
- ~~Move types out of material/model and into render.h~~
- Yeet all opengl code from UI.cPP
- Make all CAPITAL globals be not capital



#### Should do later
- Add emscripten to maketool
- Make Raycast that returns multiple points
- Add constraints to PhysicsComponents
- Models with different sets of materials
	- While doing this it would also be good to have models with more than 15 materials
- Flat shaded color material
- Textured material with alpha test but with flat color
	- Instead of dropping the fragment it would make it a flat color
- Rewrite older templates
	- Pool template is already rewritten
- Make sure that there are no duplications of typedefs
	- There are some duplications in typedefs
- Multiple player inputs
- Make constructors/destructors protected
	- Some classes should only be created in Pools
- Move GetTime() out of UI
- ~~SetScreenSize() causes a segfault~~
- Add more types to file reader/writer
	- ~~String~~
	- ~~UID~~
- PhysicsComponents can live without Entity parents.
	- Add an option to not create a MotionState
	- Add a callback function for updating

- Replace Bezier curves with Catmull-Roms
- Rewrite insides of file abstraction
	- Also add stuff for comment filtering

#### Not important
- Add rotations to sprites
- Switch out all of the std::cout's to Log's
- ~~Switch namespace Core to tram~~
- Switch the AudioComponent to using audio/source.h
- Render/Armature component linking
- Add uninit functions to all systems.
	- Add checks for them also.
	- Uniniting Core checks if everything else is also uninited
- Think of new glyphs.
	- Draw new glyphs.
- Add some more asserts to the GUI system.
- Figure out how to do resources that are generated, not loaded.
- Rewrite path following for the PhysicsComponent
- Implement navigation
- Add scripting
- Add a prefix to all header guards
- Add a better way to register entities
	- Could be a static method 'REGISTER' for each entity type
- ~~Move file.h into platform~~
- Add more rendering stuff
	- Single drawlistthingy can be in mutliple layers
	- Layers have different cameras
	- Cameras can be orthogrpahics
	- Add shear test
	- Maybe rename camera? camera is extension and in render:: there is view
	- Allow uploading of custom shaders


#### For very later versions
- Vertex animation
- 3D text component
- Tilemaps

### New features that would be nice to have, but are not very important
- ControllerComponents have configurable transitions between animations
	- In the form of:
		- State/from
		- State/to
		- Fade-out rate
		- Fade-in rate
	- Or something like that
- ControllerComponents have fake IK
	- Leg placement for creatures with legs
	- Ability to look at (turn head/etc. towards) something
- ArmatureComponents have different blending modes for animations
	- Might take some time to figure out
- AudioComponents can play more than 128 sounds at the same time
- Split animation into animation and StreamableAnimation
	- Animations that need to be streamed in and later removed have their own keyframe pools