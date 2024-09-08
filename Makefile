default: library

src_audio_audio.o: ./src/audio/audio.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/audio/audio.cpp -o src_audio_audio.o

src_audio_sound.o: ./src/audio/sound.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/audio/sound.cpp -o src_audio_sound.o

src_audio_openal_openal.o: ./src/audio/openal/openal.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/audio/openal/openal.cpp -o src_audio_openal_openal.o

src_components_animation.o: ./src/components/animation.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/components/animation.cpp -o src_components_animation.o

src_components_audio.o: ./src/components/audio.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/components/audio.cpp -o src_components_audio.o

src_components_controller.o: ./src/components/controller.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/components/controller.cpp -o src_components_controller.o

src_components_light.o: ./src/components/light.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/components/light.cpp -o src_components_light.o

src_components_particle.o: ./src/components/particle.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/components/particle.cpp -o src_components_particle.o

src_components_physics.o: ./src/components/physics.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/components/physics.cpp -o src_components_physics.o

src_components_player.o: ./src/components/player.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/components/player.cpp -o src_components_player.o

src_components_render.o: ./src/components/render.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/components/render.cpp -o src_components_render.o

src_components_sprite.o: ./src/components/sprite.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/components/sprite.cpp -o src_components_sprite.o

src_components_trigger.o: ./src/components/trigger.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/components/trigger.cpp -o src_components_trigger.o

src_entities_crate.o: ./src/entities/crate.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/entities/crate.cpp -o src_entities_crate.o

src_entities_decoration.o: ./src/entities/decoration.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/entities/decoration.cpp -o src_entities_decoration.o

src_entities_light.o: ./src/entities/light.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/entities/light.cpp -o src_entities_light.o

src_entities_marker.o: ./src/entities/marker.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/entities/marker.cpp -o src_entities_marker.o

src_entities_player.o: ./src/entities/player.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/entities/player.cpp -o src_entities_player.o

src_entities_sound.o: ./src/entities/sound.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/entities/sound.cpp -o src_entities_sound.o

src_entities_staticworldobject.o: ./src/entities/staticworldobject.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/entities/staticworldobject.cpp -o src_entities_staticworldobject.o

src_entities_trigger.o: ./src/entities/trigger.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/entities/trigger.cpp -o src_entities_trigger.o

src_extensions_camera_camera.o: ./src/extensions/camera/camera.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/camera/camera.cpp -o src_extensions_camera_camera.o

src_extensions_camera_firstperson.o: ./src/extensions/camera/firstperson.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/camera/firstperson.cpp -o src_extensions_camera_firstperson.o

src_extensions_camera_following.o: ./src/extensions/camera/following.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/camera/following.cpp -o src_extensions_camera_following.o

src_extensions_camera_fpscamera.o: ./src/extensions/camera/fpscamera.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/camera/fpscamera.cpp -o src_extensions_camera_fpscamera.o

src_extensions_camera_thirdperson.o: ./src/extensions/camera/thirdperson.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/camera/thirdperson.cpp -o src_extensions_camera_thirdperson.o

src_extensions_kitchensink_attributes.o: ./src/extensions/kitchensink/attributes.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/kitchensink/attributes.cpp -o src_extensions_kitchensink_attributes.o

src_extensions_kitchensink_character.o: ./src/extensions/kitchensink/character.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/kitchensink/character.cpp -o src_extensions_kitchensink_character.o

src_extensions_kitchensink_design.o: ./src/extensions/kitchensink/design.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/kitchensink/design.cpp -o src_extensions_kitchensink_design.o

src_extensions_kitchensink_dialog.o: ./src/extensions/kitchensink/dialog.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/kitchensink/dialog.cpp -o src_extensions_kitchensink_dialog.o

src_extensions_kitchensink_entities.o: ./src/extensions/kitchensink/entities.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/kitchensink/entities.cpp -o src_extensions_kitchensink_entities.o

src_extensions_kitchensink_inventory.o: ./src/extensions/kitchensink/inventory.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/kitchensink/inventory.cpp -o src_extensions_kitchensink_inventory.o

src_extensions_kitchensink_quest.o: ./src/extensions/kitchensink/quest.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/kitchensink/quest.cpp -o src_extensions_kitchensink_quest.o

src_extensions_kitchensink_soundtable.o: ./src/extensions/kitchensink/soundtable.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/kitchensink/soundtable.cpp -o src_extensions_kitchensink_soundtable.o

src_extensions_menu_debug.o: ./src/extensions/menu/debug.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/menu/debug.cpp -o src_extensions_menu_debug.o

src_extensions_menu_menu.o: ./src/extensions/menu/menu.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/menu/menu.cpp -o src_extensions_menu_menu.o

src_extensions_menu_system.o: ./src/extensions/menu/system.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/menu/system.cpp -o src_extensions_menu_system.o

src_extensions_scripting_lua.o: ./src/extensions/scripting/lua.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/extensions/scripting/lua.cpp -o src_extensions_scripting_lua.o

src_framework_async.o: ./src/framework/async.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/async.cpp -o src_framework_async.o

src_framework_cli.o: ./src/framework/cli.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/cli.cpp -o src_framework_cli.o

src_framework_core.o: ./src/framework/core.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/core.cpp -o src_framework_core.o

src_framework_entity.o: ./src/framework/entity.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/entity.cpp -o src_framework_entity.o

src_framework_entitycomponent.o: ./src/framework/entitycomponent.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/entitycomponent.cpp -o src_framework_entitycomponent.o

src_framework_event.o: ./src/framework/event.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/event.cpp -o src_framework_event.o

src_framework_file.o: ./src/framework/file.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/file.cpp -o src_framework_file.o

src_framework_graph.o: ./src/framework/graph.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/graph.cpp -o src_framework_graph.o

src_framework_gui.o: ./src/framework/gui.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/gui.cpp -o src_framework_gui.o

src_framework_language.o: ./src/framework/language.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/language.cpp -o src_framework_language.o

src_framework_loader.o: ./src/framework/loader.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/loader.cpp -o src_framework_loader.o

src_framework_logging.o: ./src/framework/logging.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/logging.cpp -o src_framework_logging.o

src_framework_math.o: ./src/framework/math.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/math.cpp -o src_framework_math.o

src_framework_message.o: ./src/framework/message.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/message.cpp -o src_framework_message.o

src_framework_navigation.o: ./src/framework/navigation.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/navigation.cpp -o src_framework_navigation.o

src_framework_navmesh.o: ./src/framework/navmesh.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/navmesh.cpp -o src_framework_navmesh.o

src_framework_path.o: ./src/framework/path.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/path.cpp -o src_framework_path.o

src_framework_script.o: ./src/framework/script.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/script.cpp -o src_framework_script.o

src_framework_settings.o: ./src/framework/settings.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/settings.cpp -o src_framework_settings.o

src_framework_signal.o: ./src/framework/signal.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/signal.cpp -o src_framework_signal.o

src_framework_stats.o: ./src/framework/stats.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/stats.cpp -o src_framework_stats.o

src_framework_system.o: ./src/framework/system.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/system.cpp -o src_framework_system.o

src_framework_transition.o: ./src/framework/transition.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/transition.cpp -o src_framework_transition.o

src_framework_ui.o: ./src/framework/ui.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/ui.cpp -o src_framework_ui.o

src_framework_uid.o: ./src/framework/uid.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/uid.cpp -o src_framework_uid.o

src_framework_worldcell.o: ./src/framework/worldcell.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/framework/worldcell.cpp -o src_framework_worldcell.o

src_physics_collisionmodel.o: ./src/physics/collisionmodel.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/physics/collisionmodel.cpp -o src_physics_collisionmodel.o

src_physics_physics.o: ./src/physics/physics.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/physics/physics.cpp -o src_physics_physics.o

src_physics_bullet_bullet.o: ./src/physics/bullet/bullet.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/physics/bullet/bullet.cpp -o src_physics_bullet_bullet.o

src_platform_file.o: ./src/platform/file.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/platform/file.cpp -o src_platform_file.o

src_platform_image.o: ./src/platform/image.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/platform/image.cpp -o src_platform_image.o

src_platform_time.o: ./src/platform/time.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/platform/time.cpp -o src_platform_time.o

src_platform_glfw_glfw.o: ./src/platform/glfw/glfw.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/platform/glfw/glfw.cpp -o src_platform_glfw_glfw.o

src_render_aabb.o: ./src/render/aabb.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/render/aabb.cpp -o src_render_aabb.o

src_render_animation.o: ./src/render/animation.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/render/animation.cpp -o src_render_animation.o

src_render_error.o: ./src/render/error.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/render/error.cpp -o src_render_error.o

src_render_material.o: ./src/render/material.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/render/material.cpp -o src_render_material.o

src_render_model.o: ./src/render/model.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/render/model.cpp -o src_render_model.o

src_render_render.o: ./src/render/render.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/render/render.cpp -o src_render_render.o

src_render_renderer.o: ./src/render/renderer.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/render/renderer.cpp -o src_render_renderer.o

src_render_sprite.o: ./src/render/sprite.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/render/sprite.cpp -o src_render_sprite.o

src_render_vertices.o: ./src/render/vertices.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/render/vertices.cpp -o src_render_vertices.o

src_render_opengl_error.o: ./src/render/opengl/error.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/render/opengl/error.cpp -o src_render_opengl_error.o

src_render_opengl_renderer.o: ./src/render/opengl/renderer.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/render/opengl/renderer.cpp -o src_render_opengl_renderer.o

src_render_opengl_shader.o: ./src/render/opengl/shader.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I./src -I./libraries/glfw -I./libraries/glad -I./libraries/bullet -I./libraries/lua ./src/render/opengl/shader.cpp -o src_render_opengl_shader.o

clean:
	del src_audio_audio.o
	del src_audio_sound.o
	del src_audio_openal_openal.o
	del src_components_animation.o
	del src_components_audio.o
	del src_components_controller.o
	del src_components_light.o
	del src_components_particle.o
	del src_components_physics.o
	del src_components_player.o
	del src_components_render.o
	del src_components_sprite.o
	del src_components_trigger.o
	del src_entities_crate.o
	del src_entities_decoration.o
	del src_entities_light.o
	del src_entities_marker.o
	del src_entities_player.o
	del src_entities_sound.o
	del src_entities_staticworldobject.o
	del src_entities_trigger.o
	del src_extensions_camera_camera.o
	del src_extensions_camera_firstperson.o
	del src_extensions_camera_following.o
	del src_extensions_camera_fpscamera.o
	del src_extensions_camera_thirdperson.o
	del src_extensions_kitchensink_attributes.o
	del src_extensions_kitchensink_character.o
	del src_extensions_kitchensink_design.o
	del src_extensions_kitchensink_dialog.o
	del src_extensions_kitchensink_entities.o
	del src_extensions_kitchensink_inventory.o
	del src_extensions_kitchensink_quest.o
	del src_extensions_kitchensink_soundtable.o
	del src_extensions_menu_debug.o
	del src_extensions_menu_menu.o
	del src_extensions_menu_system.o
	del src_extensions_scripting_lua.o
	del src_framework_async.o
	del src_framework_cli.o
	del src_framework_core.o
	del src_framework_entity.o
	del src_framework_entitycomponent.o
	del src_framework_event.o
	del src_framework_file.o
	del src_framework_graph.o
	del src_framework_gui.o
	del src_framework_language.o
	del src_framework_loader.o
	del src_framework_logging.o
	del src_framework_math.o
	del src_framework_message.o
	del src_framework_navigation.o
	del src_framework_navmesh.o
	del src_framework_path.o
	del src_framework_script.o
	del src_framework_settings.o
	del src_framework_signal.o
	del src_framework_stats.o
	del src_framework_system.o
	del src_framework_transition.o
	del src_framework_ui.o
	del src_framework_uid.o
	del src_framework_worldcell.o
	del src_physics_collisionmodel.o
	del src_physics_physics.o
	del src_physics_bullet_bullet.o
	del src_platform_file.o
	del src_platform_image.o
	del src_platform_time.o
	del src_platform_glfw_glfw.o
	del src_render_aabb.o
	del src_render_animation.o
	del src_render_error.o
	del src_render_material.o
	del src_render_model.o
	del src_render_render.o
	del src_render_renderer.o
	del src_render_sprite.o
	del src_render_vertices.o
	del src_render_opengl_error.o
	del src_render_opengl_renderer.o
	del src_render_opengl_shader.o

library: src_audio_audio.o src_audio_sound.o src_audio_openal_openal.o src_components_animation.o src_components_audio.o src_components_controller.o src_components_light.o src_components_particle.o src_components_physics.o src_components_player.o src_components_render.o src_components_sprite.o src_components_trigger.o src_entities_crate.o src_entities_decoration.o src_entities_light.o src_entities_marker.o src_entities_player.o src_entities_sound.o src_entities_staticworldobject.o src_entities_trigger.o src_extensions_camera_camera.o src_extensions_camera_firstperson.o src_extensions_camera_following.o src_extensions_camera_fpscamera.o src_extensions_camera_thirdperson.o src_extensions_kitchensink_attributes.o src_extensions_kitchensink_character.o src_extensions_kitchensink_design.o src_extensions_kitchensink_dialog.o src_extensions_kitchensink_entities.o src_extensions_kitchensink_inventory.o src_extensions_kitchensink_quest.o src_extensions_kitchensink_soundtable.o src_extensions_menu_debug.o src_extensions_menu_menu.o src_extensions_menu_system.o src_extensions_scripting_lua.o src_framework_async.o src_framework_cli.o src_framework_core.o src_framework_entity.o src_framework_entitycomponent.o src_framework_event.o src_framework_file.o src_framework_graph.o src_framework_gui.o src_framework_language.o src_framework_loader.o src_framework_logging.o src_framework_math.o src_framework_message.o src_framework_navigation.o src_framework_navmesh.o src_framework_path.o src_framework_script.o src_framework_settings.o src_framework_signal.o src_framework_stats.o src_framework_system.o src_framework_transition.o src_framework_ui.o src_framework_uid.o src_framework_worldcell.o src_physics_collisionmodel.o src_physics_physics.o src_physics_bullet_bullet.o src_platform_file.o src_platform_image.o src_platform_time.o src_platform_glfw_glfw.o src_render_aabb.o src_render_animation.o src_render_error.o src_render_material.o src_render_model.o src_render_render.o src_render_renderer.o src_render_sprite.o src_render_vertices.o src_render_opengl_error.o src_render_opengl_renderer.o src_render_opengl_shader.o 
	ar -crf libtramsdk.a src_audio_audio.o src_audio_sound.o src_audio_openal_openal.o src_components_animation.o src_components_audio.o src_components_controller.o src_components_light.o src_components_particle.o src_components_physics.o src_components_player.o src_components_render.o src_components_sprite.o src_components_trigger.o src_entities_crate.o src_entities_decoration.o src_entities_light.o src_entities_marker.o src_entities_player.o src_entities_sound.o src_entities_staticworldobject.o src_entities_trigger.o src_extensions_camera_camera.o src_extensions_camera_firstperson.o src_extensions_camera_following.o src_extensions_camera_fpscamera.o src_extensions_camera_thirdperson.o src_extensions_kitchensink_attributes.o src_extensions_kitchensink_character.o src_extensions_kitchensink_design.o src_extensions_kitchensink_dialog.o src_extensions_kitchensink_entities.o src_extensions_kitchensink_inventory.o src_extensions_kitchensink_quest.o src_extensions_kitchensink_soundtable.o src_extensions_menu_debug.o src_extensions_menu_menu.o src_extensions_menu_system.o src_extensions_scripting_lua.o src_framework_async.o src_framework_cli.o src_framework_core.o src_framework_entity.o src_framework_entitycomponent.o src_framework_event.o src_framework_file.o src_framework_graph.o src_framework_gui.o src_framework_language.o src_framework_loader.o src_framework_logging.o src_framework_math.o src_framework_message.o src_framework_navigation.o src_framework_navmesh.o src_framework_path.o src_framework_script.o src_framework_settings.o src_framework_signal.o src_framework_stats.o src_framework_system.o src_framework_transition.o src_framework_ui.o src_framework_uid.o src_framework_worldcell.o src_physics_collisionmodel.o src_physics_physics.o src_physics_bullet_bullet.o src_platform_file.o src_platform_image.o src_platform_time.o src_platform_glfw_glfw.o src_render_aabb.o src_render_animation.o src_render_error.o src_render_material.o src_render_model.o src_render_render.o src_render_renderer.o src_render_sprite.o src_render_vertices.o src_render_opengl_error.o src_render_opengl_renderer.o src_render_opengl_shader.o 
