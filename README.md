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

#### Small fixes
- ~~Finishing the abstract rendering interface~~
	- ~~Change uint32_t for handles and everything to a typedef!!~~
- ~~Rename PoseListObject to Pose~~
- ~~Yeet opengl_loader.cpp~~
- ~~Move armaturecomponent implementation from armature.cpp to armaturecomponent.cpp~~
- ~~Make armaturecomponent better~~
	- ~~Animations play even if started before component is loaded~~
	- ~~Callbacks on finish~~
- AudioComponents can play more than 128 sounds at the same time
	- Periodically check if an AudioComponent is 
- ~~Move CAMERA_FORWARD/UP/etc. consts into core.h~~
- ~~Add a function to audio.h to set listener orientation~~
- ~~Optimize event listeners~~
	- ~~Single pool for every types listener~~
	- ~~Very good!~~

#### For next versions
- ~~AnimationComponent~~
	- ~~Events/callbacks on animations finishing/looping~~
- ParticleComponent
	- I have no idea where I am going with it
	- Maybe add callbacks for processing the particle behaviour
- TriggerComponent
	- I have a feeling that it needs more features
	- More callbacks, maybe?
		- OnDeactivate callback
		- OnActivate callback with reference to entity that commited the activation
- Entities
	- More constructors for them
- Containers
	- Rename 'PoolAddOnly' to 'StackPool'
	- Add a PODPool
	- Add iterators to stacks
- Think of additional widgets
- Switch to using sprites for defining fonts
- Fix the mongus entity
- Add LIGMAScript


#### For versions later than next versions
- AudioComponent
	- Some sort of real-time spatial effect system
- Rendering
	- Shiny stuff shader
	- Transparency blending
	- ~~Figure out how to do different back-ends~~
	- Deferred rendering
	- ~~Layers~~
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
- Split animation into animation and StreamableAnimation
	- All of Animations share the same keyframe pool (more efficient)
	- Animations that need to be streamed in and later removed have their own pools (StreamableAnimation)

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

### Important questions that need answering
- Can you change the sun color/direction and ambient color?
- Can you change the color that is used to clear the screen?
- Can you define your own fonts?
- Can you define your own GUI skins?