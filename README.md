# Tramway Drifting and Dungeon Exploration Simulator 2022 Software Development Kit

![Tramway Drifting and Dungeon Exploration Simulator 2022 logo](/docs/logo.gif)

![Software Development Kit logo](/docs/devkit.gif)

It does what Godoesn't.

## What's it

Software library and tools for making the Tramway Drifting and Dungeon Exploration Simulator 2022 and similar applications.

It's meant to work sort of like Quake, GoldSrc or Source engines, but it uses
polygonal soup instead of BSPs and supports level streaming.

![Screenshot11](/docs/screens/screen11.png)

## Features
- Polygonal soup graphics
- Non-hierarchical scene ordering
- Ambient lighting
- Lambertian reflection
- Light source attenuation
- Colored lights and surfaces
- Phong illumination
- Point lights
- Warn's lighting for directional lights
- Level streaming
- Allows use of Quake level editors to create level geometry

![Demo1](/docs/demo1.gif)

### Links

More information can be found on the [github.io page](https://racenis.github.io/tram-sdk/). 

#### Demos

- FPS ([source code](https://github.com/racenis/jam-game) / [itch.io](https://racenis.itch.io/dziiviibas-partikula))

- Platformer ([source code](https://github.com/racenis/jam-game-ii) / [itch.io](https://racenis.itch.io/sulas-glaaze))

- Adventure ([source code](https://github.com/racenis/jam-game-iii) / [itch.io](https://racenis.itch.io/froggy-garden))

### Dependencies
Right now I'm using glad for OpenGL pointer acquisition, glfw for windowing, glm for vector math, Bullet for physics simulation. Maybe will change in the future.

### Building
I don't have any makefiles right now, so I suggest that you dump all of the code from the `src` directory into your favorite IDE.

Set the compiler include search directories to `src`, `libraries`, `libraries/glad`, `libraries/glfw` and `libraries/openal`.

For the linker, set the search directories to `libraries/binaries/win64`, if you're compiling on windows. For Linux you should install the relevant packages from your package manager. Here's the linking order that should work: `-Llibraries  -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -lglfw3 -lOpenAL32  -lglfw3 -lgdi32 -lopengl32`. The last two are for windows only, I think.

### Libraries
For convenience, I have included all of the library header files in the `libraries` directory. Since compiling all of the libraries from source takes about an hour, I have also included binaries for 64-bit windows in the `libraries/binaries/win64` directory. They should work for gcc and clang compilers.

If you're on Linux, then all of the binaries should be available to you from your package manager.

If you're using the microsoft's C++ compiler, then good luck.

### Contributions
No code contributions accepted right now, but if you find a bug or have a suggestion, then you can open a github issue.

## To-do list


#### Should do sooner

- PhysicsComponents can live without Entity parents.
	- Add an option to not create a MotionState
	- Add a callback function for updating
- Replace Bezier curves with Catmull-Roms
- ~~Refactor GLFW from UI to Platform~~
	- Also make it so that GLFW can be swapped for SDL
- Add command-line argument parser.
	- Should parse command-line arguments.
	- Also should allow setting of settings through command-line.
	- Probably can think of other useful command-line commands.
- Add a settings system.
	- Should allow saving of various settings.
	- Settings should be key-value pairs.
	- Should also allow saving of keybindings.
- Fix GUI system
	- Current GUI system was written in 3 days.
	- Add more time so that it isn't a spaghetti.
- Fix Maketool
	- Should properly seperate web and desktop builds.
- Sound table does not play if only one sound
- Add logging during render initialization end especially shader loading
- Add option to change mouse sensitivity
- Split tick event into fixed tick event and frame event
- Overhaul shader system
	- Add some kind of flags to shaders
	- These flags would inject glsl preprocessor #define's into the shader code 
- If can't find shader for a material, then print material name	
- What if the event caller gets deleted while events are being dispatched?
	- Or if messages are getting dispatched? Is it segfault?
- If you don't RegisterShader() before Render::Init(), then the Uniform blocks won't be bound and you will get pancake models
	- Add some kind of a code to bind 
- Put triangle definition into math.h
- Put MinAABB MaxAABB into there too
- And also switch traingle intersection thingy
- AddLine() drawlistentry is not being drawn on top
- Get Calc Key things does not work?? at least the distance part
- Add directional lights to lightmapper
 
#### Should do later

- Python scripting
	- Should allow accessing basic library functionality through Python.
	- Probably should have OOP abstractions and stuff.
- Object Pascal data library
	- A base that could be used for various GUI utilities in the future.
	- Should allow parsing of various library text file formats.
	- Also editing and saving.
- Project manager tool
	- Should allow editing of all the various library text file formats.
	- Should allow generation of new projects.
- Model conversion tools
	- Leverage the free Assimp library to parse various 3D model formats.
	- Convert said model formats to tram-sdk .stmdl and .dymdl formats.
	- Perhaps write Blender importers in addition to existing exporters.
- Add a better disk abstraction
	- File() should also open files that are located in compressed archives.
	- Also should open and be able to write files through HTTP.
- Default menus
	- Keybinding menu
	- Graphics menu
- Finish particles
	- Steal ideas from various particle editors
- Finish sprites
	- Make them not be distorted horizontally in the shader
- Swapable physics engine?

#### For very later versions
- Occlusion/frustrum culling
- Add rotations to sprites
- Switch out all of the std::cout's to Log's
- Think of new glyphs.
	- Draw new glyphs.
- Add some more asserts to the GUI system.
- Rewrite path following for the PhysicsComponent
- Implement navigation
- Add more rendering stuff
	- Single drawlistthingy can be in mutliple layers
	- Layers have different cameras
	- Cameras can be orthogrpahics
	- Add shear test
	- Maybe rename camera? camera is extension and in render:: there is view
- Make the engine work without any files
	- That means that it will run just fine without any files on the disk
- Fix animation scaling translation
	- There's something weird going on if you both scale and translate in an animation
- Make Raycast that returns multiple points
	
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
- Actually it might be a better idea to delegate this functionality to a seperate component.
- Input recording and playback
- AudioComponents can play more than 128 sounds at the same time
- Split animation into animation and StreamableAnimation
	- Animations that need to be streamed in and later removed have their own keyframe pools
- Sprite batching
- Vertex animation
- 3D text component
- Tilemaps