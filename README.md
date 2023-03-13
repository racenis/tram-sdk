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

#### Should do soonest
- Add a thing that lets you see how many things are in the loader queues
- Add a deactivation callback to triggers

#### Should do sooner
- Keybinding menu
- Graphics menu
- Add a prefix to all header guards
- Add screen clear color to render
- Add font definitions to Ext::Menu
- Add a better way to register entities
	- Could be a static method 'REGISTER' for each entity type
- Rewrite MaterialInfo
	- Automatically add .list extension to filename
	- Automatically add /data/ directory prefix to filename
	- Add a header
- Put all cells in a seperate directory
	- Like it is done with sprites, models, animations, etc.
- Replace header in worldcell files
	- Should be like in model/animation/etc. files


#### Should do later
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
- SetScreenSize() causes a segfault
- Add more types to file reader/writer
	- String
	- UID

#### Not important
- Switch out all of the std::cout's to Log's
- Switch namespace Core to tram
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