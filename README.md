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
#### Version 0.0.4
- Switch out all of the std::cout's to Log's
- Finish writing the report
	- Test performance of the engine
	- Finish the platform/file.cpp implementation
	- Switch all file loading to file.h abstraction
	- Test performance again
- Finish the Stats system
- Switch namespace Core to tram
- Move language stuff out of the framework/etc.h/etc.cpp
- Make system reject register above 64 systems
- Finish file writer
- Rewrite render sort key generator to use bitfields
- Add input polling functions to UI
- Render/Armature component linking

#### Version 0.0.5
- Sprites
	- Add option to load frames from disk
- Fonts/glyphs 
	- Add an option to register custom fonts
	- Add a typedef font_t
	- Function font_t RegisterFont (type regular/msdf, font_t)
- Switch to using sprites
	- Instead of feeding it a font index, instead feeda font_t

#### Version 0.0.6
- New CharacterController
	- Switch from RigidBody to Kinetic
- Partially rewrite the PhysicsComponent
	- Delete unused methods
	- Add more methods

#### Version 0.0.7
- Rewrite path following for the PhysicsComponent
- Implement navigation

#### Version 0.0.8
- Add scripting


#### For later versions
- AudioComponent
	- Some sort of real-time spatial effect system
- Rendering
	- Shiny stuff shader
	- Transparency blending
	- Deferred rendering
	- Occlusion culling
	- Bump and specularity maps
- Logging redirection
- Fix the tramway entity
- Fix the bus entity
- ParticleComponent
	- Animated particles
- Create a custom 16-bit character encoding
- Change font rendering to use sprite system
- Optimize the model material bucketing system
- Make the player controller more responsive
- Physics cached collision primitives
- Add a proper third person camera 
- Put LODs into files
- Add some generic gameplay systems
	- Tools/items
	- Item management/inventory
	- Dialog
	- Cutscenes
	- AI
- Async job system
- Oof
	- Sprite
		- Loading frames from a file
		- Alternate [???]


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