cmake_minimum_required(VERSION 3.2)
project(tramsdk VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# +----------------------------------------------------------------------------+
# |                                                                            |
# |                               CONFIGURATION                                |
# |                                                                            |
# +----------------------------------------------------------------------------+

# can select only one
option(RENDER_OPENGL "Use the OpenGL renderer (default)" ON)
option(RENDER_DIRECT3D "Use the Direct3D renderer" OFF)
option(RENDER_SOFTWARE "Use Software rendering" OFF)

# not really an option
option(PHYSICS_BULLET "Use the Bullet physics (default)" ON)

# can select only one
option(PLATFORM_GLFW "Use the GLFW platform (default)" ON)
option(PLATFORM_SDL "Use the SDL platform" OFF)

# can select only one
# only openal actually works
option(AUDIO_OPENAL "Use the OpenAL backend (default)" ON)
option(AUDIO_SPATIAL "Use the Spatial backend" OFF)
option(AUDIO_TEMPLATE "Use the Template backend" OFF)

# can select whichever
option(EXTENSION_CAMERA "Use the Camera extension" ON)
option(EXTENSION_MENU "Use the Menu extension" ON)
option(EXTENSION_LUA "Use the Lua extension" ON)
option(EXTENSION_KITCHENSINK "Use the Kitchensink extension" ON)

# +----------------------------------------------------------------------------+
# |                                                                            |
# |                                 FRAMEWORK                                  |
# |                                                                            |
# +----------------------------------------------------------------------------+

file(GLOB_RECURSE SOURCES 	${PROJECT_SOURCE_DIR}/src/components/*.cpp
							${PROJECT_SOURCE_DIR}/src/framework/*.cpp
							${PROJECT_SOURCE_DIR}/src/entities/*.cpp)

# +----------------------------------------------------------------------------+
# |                                                                            |
# |                                   AUDIO                                    |
# |                                                                            |
# +----------------------------------------------------------------------------+

file(GLOB AUDIO_SOURCES ${PROJECT_SOURCE_DIR}/src/audio/*.cpp)
list(APPEND SOURCES ${AUDIO_SOURCES})

if(AUDIO_OPENAL)
	message(STATUS "Using OpenAL audio backend")
	
	if(AUDIO_SPATIAL OR AUDIO_TEMPLATE)
		message(FATAL_ERROR "Multiple audio backends selected!")
	endif()
	
	file(GLOB_RECURSE OPENAL_SOURCES ${PROJECT_SOURCE_DIR}/src/audio/openal/*.cpp)
	list(APPEND SOURCES ${OPENAL_SOURCES})
endif()

if(AUDIO_SPATIAL)
	message(STATUS "Using OpenGL rendering backend")
	
	if(AUDIO_OPENAL OR AUDIO_TEMPLATE)
		message(FATAL_ERROR "Multiple audio backends selected!")
	endif()
	
	file(GLOB_RECURSE SPATIAL_SOURCES ${PROJECT_SOURCE_DIR}/src/audio/spatial/*.cpp)
	list(APPEND SOURCES ${SPATIAL_SOURCES})
endif()

if(AUDIO_TEMPLATE)
	message(STATUS "Using OpenGL rendering backend")
	
	if(AUDIO_OPENAL OR AUDIO_SPATIAL)
		message(FATAL_ERROR "Multiple audio backends selected!")
	endif()
	
	file(GLOB_RECURSE AUDIO_TEMPLATE_SOURCES ${PROJECT_SOURCE_DIR}/src/audio/template/*.cpp)
	list(APPEND SOURCES ${AUDIO_TEMPLATE_SOURCES})
endif()

# +----------------------------------------------------------------------------+
# |                                                                            |
# |                                  PHYSICS                                   |
# |                                                                            |
# +----------------------------------------------------------------------------+

file(GLOB PHYSICS_SOURCES ${PROJECT_SOURCE_DIR}/src/physics/*.cpp)
list(APPEND SOURCES ${PHYSICS_SOURCES})

if(PHYSICS_BULLET)
	message(STATUS "Using Bullet physics backend")
	
	include_directories(${PROJECT_SOURCE_DIR}/libraries/bullet)
	
	file(GLOB_RECURSE BULLET_SOURCES ${PROJECT_SOURCE_DIR}/src/physics/bullet/*.cpp)
	list(APPEND SOURCES ${BULLET_SOURCES})
endif()

# +----------------------------------------------------------------------------+
# |                                                                            |
# |                                   RENDER                                   |
# |                                                                            |
# +----------------------------------------------------------------------------+

file(GLOB RENDER_SOURCES ${PROJECT_SOURCE_DIR}/src/render/*.cpp)
list(APPEND SOURCES ${RENDER_SOURCES})

if(RENDER_OPENGL)
	message(STATUS "Using OpenGL rendering backend")
	
	if(RENDER_DIRECT3D OR RENDER_SOFTWARE)
		message(FATAL_ERROR "Can't use Direct3D or Software, OpenGL already selected!")
	endif()
	
	file(GLOB_RECURSE OPENGL_SOURCES ${PROJECT_SOURCE_DIR}/src/render/opengl/*.cpp)
	list(APPEND SOURCES ${OPENGL_SOURCES})
endif()

if(RENDER_DIRECT3D)
	message(STATUS "Using Direct3D rendering backend")
	
	# TODO: maybe add a check to check if this is not being compiled on Linux?
	
	if(RENDER_OPENGL OR RENDER_SOFTWARE)
		message(FATAL_ERROR "Can't use OpenGL or Software, Direct3D already selected!")
	endif()
	
	if(NOT PLATFORM_SDL)
		message(FATAL_ERROR "Direct3D render backend only works with SDL!")
	endif()
	
	file(GLOB_RECURSE DIRECT3D_SOURCES ${PROJECT_SOURCE_DIR}/src/render/direct3d/*.cpp)
	list(APPEND SOURCES ${DIRECT3D_SOURCES})
endif()

if(RENDER_SOFTWARE)
	message(STATUS "Using Software rendering backend")
	
	if(RENDER_OPENGL OR RENDER_DIRECT3D)
		message(FATAL_ERROR "Can't use OpenGL or Direct3D, Software already selected!")
	endif()
	
	if(NOT PLATFORM_SDL)
		message(FATAL_ERROR "Software render backend only works with SDL!")
	endif()
	
	file(GLOB_RECURSE SOFTWARE_SOURCES ${PROJECT_SOURCE_DIR}/src/render/software/*.cpp)
	list(APPEND SOURCES ${SOFTWARE_SOURCES})
endif()


# +----------------------------------------------------------------------------+
# |                                                                            |
# |                                 PLATFORM                                   |
# |                                                                            |
# +----------------------------------------------------------------------------+

file(GLOB PLATFORM_SOURCES ${PROJECT_SOURCE_DIR}/src/platform/*.cpp)
list(APPEND SOURCES ${PLATFORM_SOURCES})

if(PLATFORM_GLFW)
	message(STATUS "Using GLFW backend")
	
	if(PLATFORM_SDL)
		message(FATAL_ERROR "Can't use SDL, GLFW already selected!")
	endif()
	
	include_directories(${PROJECT_SOURCE_DIR}/libraries/glfw)
	
	file(GLOB_RECURSE GLFW_SOURCES ${PROJECT_SOURCE_DIR}/src/platform/glfw/*.cpp)
	list(APPEND SOURCES ${GLFW_SOURCES})
endif()

if(PLATFORM_SDL)
	message(STATUS "Using SDL backend")
	
	if(PLATFORM_GLFW)
		message(FATAL_ERROR "Can't use GLFW, SDL already selected!")
	endif()
	
	include_directories(${PROJECT_SOURCE_DIR}/libraries/sdl2)
	
	file(GLOB_RECURSE SDL_SOURCES ${PROJECT_SOURCE_DIR}/src/platform/sdl/*.cpp)
	list(APPEND SOURCES ${SDL_SOURCES})
endif()

# +----------------------------------------------------------------------------+
# |                                                                            |
# |                                EXTENSIONS                                  |
# |                                                                            |
# +----------------------------------------------------------------------------+

if(EXTENSION_CAMERA)
	message(STATUS "Using Camera extension")
	file(GLOB_RECURSE CAMERA_SOURCES ${PROJECT_SOURCE_DIR}/src/extensions/camera/*.cpp)
	list(APPEND SOURCES ${CAMERA_SOURCES})
endif()

if(EXTENSION_MENU)
	message(STATUS "Using Menu extension")
	file(GLOB_RECURSE MENU_SOURCES ${PROJECT_SOURCE_DIR}/src/extensions/menu/*.cpp)
	list(APPEND SOURCES ${MENU_SOURCES})
endif()

if(EXTENSION_LUA)
	message(STATUS "Using Lua extension")
	
	include_directories(${PROJECT_SOURCE_DIR}/libraries/lua)
	
	file(GLOB_RECURSE LUA_SOURCES ${PROJECT_SOURCE_DIR}/src/extensions/scripting/*.cpp)
	list(APPEND SOURCES ${LUA_SOURCES})
endif()

if(EXTENSION_KITCHENSINK)
	message(STATUS "Using Kitchensink extension")
	file(GLOB_RECURSE KITCHENSINK_SOURCES ${PROJECT_SOURCE_DIR}/src/extensions/kitchensink/*.cpp)
	list(APPEND SOURCES ${KITCHENSINK_SOURCES})
endif()

# +----------------------------------------------------------------------------+
# |                                                                            |
# |                               CONFIGURATION                                |
# |                                                                            |
# +----------------------------------------------------------------------------+

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/libraries)

# I think that glad is only needed on Windows?
# TODO: check
include_directories(${PROJECT_SOURCE_DIR}/libraries/glad)

add_library(tramsdk STATIC ${SOURCES})
