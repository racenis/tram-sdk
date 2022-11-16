# Tramway Drifting and Dungeon Exploration Simulator 2022 Software Development Kit

![Tramway Drifting and Dungeon Exploration Simulator 2022 logo](/docs/logo.gif)

![Software Development Kit logo](/docs/devkit.gif)

## What's it

Software library and tools for making the Tramway Drifting and Dungeon Exploration Simulator 2022.

![Software Development Kit logo](/docs/screen11.png)

### How finished is it?

It's usable, but the programming interfaces aren't very stable. User experience also is very lacking.

### Links

More information and also screenshots can be found on the [github.io page](https://racenis.github.io/tram-sdk/). 

Instruction manual and other documentations [can be found here](https://racenis.github.io/tram-sdk/manual/). 

## To-do list

- Remove Uninit() method from all components
	- Move the code into the destructor
- Remove SetParameters() from Entity
	- Move it into the constructor
- Remove Yeet() from Entity
	- Move the code into the destructor
- Convert name_t into a class
- Remove SetErrorModel() and SetErrorMaterial()
	- Move the code into Render::Init()

#### For next versions
- ParticleComponent
	- I have no idea where I am going with it
	- Maybe add callbacks for processing the particle behaviour
- TriggerComponent
	- I have a feeling that it needs more features
	- More callbacks, maybe?
		- OnDeactivate callback
		- OnActivate callback with reference to entity that commited the activation
- Think of additional GUI widgets
- Switch to using sprites for defining fonts
- Add LIGMAScript
- ~~Add a ComponentProxy~~
	- ~~Automatically removes components~~


#### For versions later than next versions
- AudioComponent
	- Some sort of real-time spatial effect system
- Rendering
	- Shiny stuff shader
	- Transparency blending
	- Deferred rendering
	- Occlusion culling
	- Bump and specularity maps
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

### Important questions that need answering
- Can you change the sun color/direction and ambient color?
- Can you change the color that is used to clear the screen?
- Can you define your own fonts?
- Can you define your own GUI skins?