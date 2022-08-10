# Tramway Drifting and Dungeon Exploration Simulator 2022 Software Development Kit

![Tramway Drifting and Dungeon Exploration Simulator 2022 logo](/docs/logo.gif)

![Software Development Kit logo](/docs/devkit.gif)

## What's it

Software library and tools for making the Tramway Drifting and Dungeon Exploration Simulator 2022.

![Software Development Kit logo](/docs/screen11.png)

More information and also screenshots can be found on the [github.io page](https://racenis.github.io/tram-sdk/). 

Instruction manual and other documentations [can be found here](https://racenis.github.io/tram-sdk/manual/). 

## To-do list

 - rename pooladdonly->stackpool
 	- rename
 	- fix compiler errors
 - fix the animations
	- add bone roll to the dynamic model exporter
	- add bone roll to the bone definition
	- add bone roll to the model importer
		- fix up old dynamic models
	- replace the bone space matrix calculation
	- re-export mongus and his animations and test them
 - ~~new immediate mode gui system~~
	- ~~add non-msdf gui material~~
	- ~~add frame stack~~
	- ~~add a clipping glyph setter~~
	- ~~add scrolling frames~~
	- ~~add texts~~
	- ~~add buttons~~
	- ~~add dropdown selections~~
	- ~~add text boxes~~
 - optimize the model material bucketing system
 - make the player controller more responsive
 - add a proper third person camera
 - add some generic gameplay systems
	- tools/items
	- item management/inventory
	- dialog
	- cutscenes
	- ai
 - make the level editor
	- make the ui
	- integrate it with engine
	- make tools
		- bezier path tool
		- level transition tool
		- occlusion tool
		- navmesh tool
 - async job system
	- navmesh path finder
 - occlusion culling
 - advanced rendering
	- layers
 - advanced effects
	- texture transparency
	- shiny materials
	- bump maps
	- particles
	- sprites
 - webgl support
 - test custom container templates
