# Tramway Drifting and Dungeon Exploration Simulator 2022 Software Development Kit

![Tramway Drifting and Dungeon Exploration Simulator 2022 logo](/docs/logo.gif)

![Software Development Kit logo](/docs/devkit.gif)

It does what Godoesn't.

I am not reinventing the wheel, I am disrupting the wheel industry.

Free func_door included with every install!

![Screenshot11](/docs/screens/screen11.png)

## What's it

Software library and tools for making the [Tramway Drifting and Dungeon Exploration Simulator 2022](https://racenis.itch.io/tramway-drifting-and-dungeon-exploration-simulator-2022) and similar applications.

It's meant to work sort of like Quake, GoldSrc or Source engines, but it uses
polygonal soup instead of BSPs and supports level streaming.

![EditingApp](/docs/images/etc/editingapp.png)

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

![Teapot](/docs/images/etc/teapot.gif)

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
- Python 3.8 for scripts
- Blender 2.83 for 3D model and animation exporters

![Demo1](/docs/demo1.gif)

### Links

More information can be found on the [github.io page](https://racenis.github.io/tram-sdk/). 

#### Demos

- FPS ([source code](https://github.com/racenis/jam-game) / [itch.io](https://racenis.itch.io/dziiviibas-partikula))

- Platformer ([source code](https://github.com/racenis/jam-game-ii) / [itch.io / web demo](https://racenis.itch.io/sulas-glaaze))

- Adventure ([source code](https://github.com/racenis/jam-game-iii) / [itch.io / web demo](https://racenis.itch.io/froggy-garden))

#### Other tools

- [Applets](https://github.com/racenis/tram-applets), i.e. GUI data editors

- [Level editor](https://github.com/racenis/tram-world-editor) for editing worldcells

#### Templates

- [Teapot explorer](https://github.com/racenis/tram-template), main template project

#### Examples

- Pongpot ([source code](https://github.com/racenis/tram-example/tree/main/example-ping-pong))
- ([All examples](https://github.com/racenis/tram-example))

![PongPot](/docs/images/etc/pongpot.gif)

### Building
#### Windows / MinGW
Edit the settings found in `maketool.ini`. Set the platform to WIN32 or WIN64, 
depending on your compiler version.

If you compile dependencies yourself, place them in the `/libraries/binaries/win32`
or `/libraries/binaries/win64` directories.

Set repository root as the working directory.

Run `devtools/maketool.py generate` to generate a `Makefile`.

Use `make library && make clean` to compile the library.

#### Linux / GCC

Install Python interpreter, C++ compiler and OpenGL headers.
```
sudo apt-get install python3
sudo apt-get install build-essential
sudo apt-get install libmesa-dev
```
Install dependencies.
```
sudo apt-get install libglfw3-dev
sudo apt-get install libbullet-dev
sudo pat-get install libopenal-dev
sudo apt-get install liblua5.4-dev
```
These are the packages for Debian-based distributions, for other distributions
check your package manager. 

Download the source code.
```
git clone https://github.com/racenis/tram-sdk.git
```

`vi maketool.ini` or `nano maketool.ini`
Edit the makefile generator's configuration. Set the `compiler` to your `g++`
compiler and `archiver` to your `ar` program. Set `platform` to `LINUX`. Select
the modules which you will be using.

Review the configuration and generate the makefile.
```
chmod +x devtools/maketool.py
devtools/maketool.py config
devtools/maketool.py generate
```

Then compile.
```
make
```

### Dependencies

Header files and binaries (for MinGW x86/x86_64 and WebAssembly) included in `libraries` directory.
Check [/libraries/README.md](/libraries/README.md) for more information.

### Contributions

Due to risks associated with LLM use, we are not accepting code contributions
for the runtime (this repository) part of the project. LLMs and other machine
learning based coding tools exhibit a propensity for emitting code that is
incompatible with the MIT license. Alas, there is no good way to separate genuine
human code from AI slop, ergo we will not be able to accept any code pull
requests for this repository.

However, you can still help by submitting bug reports or suggestions for
improvements.

#### Why not accepting code
Consider this example: you use this framework to create a project. Maybe due to
the platform that you are using, or due to some other requirement you are also
using a third party closed source library.

Later it is determined that a contributor has added GPL licensed code to the
framework. Now you can either release the source code to your project and
violate the third party's license or you keep the source closed and violate the
GPL.

Either way, the outcome is doubleplus ungood.

## Milestones

### Getting last major features in

Currently we have most of the planned features for v1.0.0, after which the core
framework should be mostly feature-complete.

In the future we'll keep tweaking the core framework, but most of the efforts
will be focused on additional backends, integrations and extensions.

### Developing professional-grade projects

We have several projects in the works and they all use this framework. We'll
get them to at least prototype or demo state.

This will allow us to find additional deficiencies in the framework and help
ensure that the framework has all the features needed for full productions.

### Final touches and extensive testing

We'll finish all planned changes that will break the API.

We'll finally set up automated testing, some kind of CI, dockerized
cross-platform builds and automated release builds.

This should help us track down most of remaining bugs and ensure that we catch
any regressions.

### Switching to proper development practices

This means semantic versioning, proper change logs and possibly even supporting
multiple branches.

## To-do list

#### For v0.1.2

- Finish particles
	- Steal ideas from various particle editors
- Finish sprites
	- Make them not be distorted horizontally in the shader
	- Add rotations
- Add entity that shows messages in 3D space
	- Just calls DebugText
	- Can be toggled on/off
	- Can have a distance set
- Material editor GUI applet
- Particle editor GUI applet
- Sprite editor GUI applet
- Model/animation viewer GUI applet
- Allow setting alpha for any model
	- Make sure that vec4 color gets passed into API from Render/MeshComponents
	- Make sure that it makes its way into shaders
	- Add a renderflag to enable alpha blending
	- Merge _blend.frag files into .frag files
	- Consider moving _alpha.frag from separate file to compile flag
- Allow packing additional textures
	- Like color texture + normal or specular map
	- Allow more than 15 textures for OpenGL API
	- Also add separate texture array sampler for lightmaps/environment maps
- Add some more asserts to the GUI system
	- Like when pushing/popping, changing state, etc.
- Make the engine work without any files
	- That means that it will run just fine without any files on the disk
	
#### For v0.2.0

- Plugins
	- Essentially a .zip file with `data/`, `scripts/` and `shaders/` directories
	- Would allow easy DLC/mods/etc.
	- Loaded as either directories or archives
- Finish serialization
	- Allow re-loading WorldCells
	- Serialize WorldCell state to disk
		- Includes all entities
	- Save and load settings
	- Deserialize WorldCell state from disk
- Network replication
	- Allow basic entities to be replicated over network
	- Also custom data
	- We can always upgrade the protocol later
- Launcher GUI
	- Allows configuring settings
	- Reordering plugins
	- Changing language
	- Selecting executable to run
- Add SoundGraph features to level editor
	- Allow tagging nodes as portals
	- Add effect parameters
	- Automatically derive some parameters
		- Like reverb/echo
	- Automatically generate links between nodes
		- Also discover rooms and link room nodes together
- Add SoundGraph stuff to AudioComponent
	- Will need to add OpenAL plugin
	- Like the one that supports various effects
- Add Navmesh features to level editor
	- Automatically connect nodes
	- Add parameter that determines max width/height
- Implement Navmesh follower
	- Can add flags
		- Avoid obstacles
		- Discovers obstacles
		- Ignores directions
		- Etc.
- Additional Navmesh features
	- Objects can temporarily block navmesh nodes
	- Agents will route around them
- Planar reflections
	- Setup special material
	- Add reflection plane
	- Only single reflection plane rendered
		- Use environment map as a backup
	- Polygons near reflection plane
	- For OpenGL
		- Add reflection pane
		- Render upside-down + clipped scene
		- Render in reflective surfaces
		- Render rest of the scene

#### For v1.0.0

- Add some kind of crash reporting
- Make sure that GUI programs work on Linux
- Finally add Macintosh support for everything
- Unit tests
	- Improve DecalComponent
		- Attach it to RenderComponents
			- When they move, it moves
		- Allow attachments to posed objects
		- Maybe modify SceneTree
			- Allow entities to subscribe to RenderComponent movements
		- Maybe implement decal batching
			- Multiple decals on same RenderComponent 
	- AnimationComponent
		- Use file mock
		- Parse some in-memory animation files
		- Check if the pose matrices more or less correct
		- Also check detaching bones
	- AudioComponent
		- Use mock for backend
		- Component and Integration trivial
		- Test SoundGraph!!
	- MeshComponent
		- Mock render backend
		- Just check if a couple of triangles get copied correctly
	- ParticleComponent
		- Just some smoke tests
	- SpriteComponent
		- Just check that it clips stuff correctly from sprite sheets
	- Scripting
		- Just go through all Lua functions and check that they don't crash
		- Maybe some math functions we could test a little more
	- Async
		- Might be a good idea to mock some components and test it
- Integration tests
	- Rendering
		- Use the low-level render API to test integration
		- Ideas: untextured cube, shiny sphere, lights, no lights, etc.
		- Render to image
		- Compare images to previously correct renders for regressions
		- Use OpenGL/Vulkan/Direct3D emulators
- Scripts for running all tests automatically
- Dockerfiles for running all tests in a container
- Additional dockerfiles
	- Compiling framework
	- Compiling level editor
	- Compiling GUI applets
- Fully automated builds
- Final code pass
	- Remove old comments
	- Check if comments are still relevant
	- Expand and check Doxygen comments
- Final documentation pass
	- Expand the documentation
	- Double check it

#### Bug fixes
- Sound table does not play if only one sound
- Issue for Direct3D on some hardware
	- When you see it, you see it
	- Only happens on some hardware, hard to replicate
	- Try using Direct3D wrappers like WINE or dgVoodoo
- Fix animation scaling translation
	- There's something weird going on if you both scale and translate in an animation


### New features that would be nice to have, but are not very important
- Improve GUI::Textbox
	- Add cursor movement
	- Add selection
	- Add copy-pasting
- Python scripting
	- Should allow accessing basic library functionality through Python.
	- Probably should have OOP abstractions and stuff.
- Model conversion tools
	- Leverage the free Assimp library to parse various 3D model formats.
	- Convert said model formats to tram-sdk .stmdl and .dymdl formats.
	- Perhaps write Blender importers in addition to existing exporters.
- Occlusion culling
- AnimationComponent multi-threading
	- Could improve performance a bit
	- Need to do more extensive profiling
- Input recording and playback
	- Would need to make sure that we can create deterministic logic
	- Then we would just record keyboard/mouse events
	- Would also allow inserting previously recorded events
- AudioComponents can play more than 128 sounds at the same time
- Sprite batching
	- Currently each SpriteComponent gets its own vertex buffer
	- We would put all sprites that share a material in the same vertex buffer
- Vertex animation
	- Accomplished through blend shapes
- 3D text component
	- Generates a mesh that spells out a text
- Tilemaps
	- Load in a tilemap format
		- Maybe one of the ones used by Tiled
	- Generate a mesh for rendering and maybe collisions
	- Allow dynamic modifications of tilemap
- Achievements and social features
	- Will need to check what different stores and platforms offer
	- Ideally the API would allow code to be written once and work everywhere
