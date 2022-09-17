# Tramway Drifting and Dungeon Exploration Simulator 2022 Software Development Kit

![Tramway Drifting and Dungeon Exploration Simulator 2022 logo](/docs/logo.gif)

![Software Development Kit logo](/docs/devkit.gif)

## What's it

Software library and tools for making the Tramway Drifting and Dungeon Exploration Simulator 2022.

![Software Development Kit logo](/docs/screen11.png)

More information and also screenshots can be found on the [github.io page](https://racenis.github.io/tram-sdk/). 

Instruction manual and other documentations [can be found here](https://racenis.github.io/tram-sdk/manual/). 

## To-do list

#### For version 0.0.1
- ~~Refactor LightComponent.h~~ oops, forgot
- ~~Re-check documentation~~

#### For next versions
- AnimationComponent
	- ~~Make the code more readable~~
	- ~~Blend weights~~
	- ~~Different speeds~~
	- Pause/resume
		- Pausing on the last frame
	- Fade in/out
	- Events on animations finishing/looping
	- Other keyframe interpolation options
- ControllerComponent
	- ~~Remember actions between frames~~
	- Configurable transitions between animations
- ~~Add an AudioComponent~~
	- ~~Pause/resume~~
	- ~~Sounds larger than 32KB~~
- ~~SpriteComponent~~
	- ~~Better system of defining frames~~
	- ~~Multiple frame lists per sprite~~ canceled
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
- Fix the mongus entity
- Add LIGMAScript

#### For versions later than next versions
- AudioComponent
	- Some sort of real-time spatial effect system
- Rendering
	- Shiny stuff shader
	- Transparency blending
	- Figure out how to do different back-ends
	- Deferred rendering
	- Layers
	- Occlusion culling
	- Bump and specularity maps
- Fix the tramway entity
- Fix the bus entity
- SpriteComponent
	- Batching
- ParticleComponent
	- Batching together with sprites
	- Animated particles
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

#### For previouse versions
 - ~~fix the animations~~
	- ~~add bone roll to the dynamic model exporter~~
	- ~~add bone roll to the bone definition~~
	- ~~add bone roll to the model importer~~
		- ~~fix up old dynamic models~~
	- ~~replace the bone space matrix calculation~~
	- ~~re-export mongus and his animations and test them~~
 - ~~new immediate mode gui system~~
	- ~~add non-msdf gui material~~
	- ~~add frame stack~~
	- ~~add a clipping glyph setter~~
	- ~~add scrolling frames~~
	- ~~add texts~~
	- ~~add buttons~~
	- ~~add dropdown selections~~
	- ~~add text boxes~~
 - ~~advanced rendering effects~~
	- ~~particles~~
	- ~~sprites~~
 - ~~make the level editor~~
	- ~~make the ui~~
	- ~~integrate it with engine~~
	- ~~make tools~~
		- ~~bezier path tool~~
		- ~~level transition tool~~
		- ~~navmesh tool~~