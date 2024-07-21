# Tramway Drifting and Dungeon Exploration Simulator 2022 Software Development Kit

![Tramway Drifting and Dungeon Exploration Simulator 2022 logo](/docs/logo.gif)

![Software Development Kit logo](/docs/devkit.gif)

It does what Godoesn't.

Free func_door included with every install!

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

### Platforms
- Windows (XP SP3 or later)
- Linux
- Web browser (needs WebAssembly support)

### Requirements
- Graphics card (optional) that supports either:
	- OpenGL 4.0
	- WebGL 2.0 (for web builds)
	- Direct3D 9.0
- Decent CPU (at least Pentium 4) if using software rendering
- Mouse and/or keyboard
- Will probably add controllers and other input devices later
- GCC/G++ 12.4.0 or MinGW/G++ 12.4.0
- Clang should work too
- Don't know about MSVC++
- If it supports C++20, it should work


![Demo1](/docs/demo1.gif)

### Links

More information can be found on the [github.io page](https://racenis.github.io/tram-sdk/). 

#### Demos

- FPS ([source code](https://github.com/racenis/jam-game) / [itch.io](https://racenis.itch.io/dziiviibas-partikula))

- Platformer ([source code](https://github.com/racenis/jam-game-ii) / [itch.io / web demo](https://racenis.itch.io/sulas-glaaze))

- Adventure ([source code](https://github.com/racenis/jam-game-iii) / [itch.io / web demo](https://racenis.itch.io/froggy-garden))

### Building
#### MinGW/GCC/Clang(?)/Emscripten/
Edit the settings found in `maketool.ini`.

Set repository root as the working directory.

Run `devtools/maketool.py generate` to generate a `Makefile`.

Use `make library && make clean` to compile the library.

### Dependencies
Header files and binaries (for MinGW x86/x86_64 and WebAssembly) included in `libraries` directory.
Check [/libraries/README.md](/libraries/README.md) for more information.

### Contributions
No code contributions accepted right now, but if you find a bug or have a suggestion, then you can open a github issue.

## To-do list


#### Should do sooner

- RenderComponents should support offsetting textures
	- A method like RenderComponent::SetTextureOffset(Material*, vec2).
	- It would add an offset to the specified Materia's texture.
	- This would allow texture animations and the like.
- Replace Bezier curves with Catmull-Roms
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
- And also switch triangle intersection thingy
- AddLine() drawlistentry is not being drawn on top
- Add directional lights to lightmapper
- Hashmap deletion does not get actually deleted?
- Switch hashmap to uint32_t
 
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
	- Single drawlistthingy can be in multiple layers
	- Layers have different cameras
	- Add shear test
- Make the engine work without any files
	- That means that it will run just fine without any files on the disk
- Fix animation scaling translation
	- There's something weird going on if you both scale and translate in an animation
- Make Raycast that returns multiple points
	
### New features that would be nice to have, but are not very important
- Animation state machine component for ControllerComponents
	- Would have the same or similar API as ControllerComponent
	- Would allow setting transitions between animations in the form of:
		- State/from
		- State/to
		- Fade-out rate
		- Fade-in rate
	- Or something like that
- Input recording and playback
- AudioComponents can play more than 128 sounds at the same time
- Split animation into animation and StreamableAnimation
	- Animations that need to be streamed in and later removed have their own keyframe pools
- Sprite batching
- Vertex animation
- 3D text component
- Tilemaps