cmake_minimum_required(VERSION 123.20)
# TODO: update the list of all of the things that need be compile
#cmake_minimum_required(VERSION 3.20)
project(tram)

set(CMAKE_CXX_STANDARD 20)

include_directories(src/)
include_directories(src/entities)
include_directories(src/render)
include_directories(libraries)

add_executable(tram
        src/entities/crate.cpp
		src/entities/lamp.cpp
        src/entities/player.cpp
        src/entities/staticworldobject.cpp
        src/render/glyphs.cpp
        src/render/material.cpp
        src/render/model.cpp
        src/render/render_misc.cpp
        src/async.cpp
        src/async.h
        src/core.cpp
        src/core.h
        src/entity.cpp
        src/entities.h
        src/entitycomponents.cpp
        src/entitycomponents.h
        src/main.cpp
        src/net.cpp
        src/net.h
        src/physics.cpp
        src/physics.h
        src/render.cpp
        src/render.h
        src/shader.cpp
        src/ui.cpp
        src/ui.h
        src/worldcell.cpp
		src/serializeddata.h)#this list is extremely out of date

target_link_libraries(tram
        ${CMAKE_CURRENT_SOURCE_DIR}/libraries/libBulletSoftBody.a
        ${CMAKE_CURRENT_SOURCE_DIR}/libraries/libBulletDynamics.a
        ${CMAKE_CURRENT_SOURCE_DIR}/libraries/libBulletCollision.a
        ${CMAKE_CURRENT_SOURCE_DIR}/libraries/libLinearMath.a
        ${CMAKE_CURRENT_SOURCE_DIR}/libraries/libglfw3.a #this part needs also audio linking
        )
