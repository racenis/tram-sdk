default: project

src_main.o: ./src/main.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/main.cpp -o src_main.o

src_platform_time.o: ./src/platform/time.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/platform/time.cpp -o src_platform_time.o

src_platform_image.o: ./src/platform/image.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/platform/image.cpp -o src_platform_image.o

src_platform_file.o: ./src/platform/file.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/platform/file.cpp -o src_platform_file.o

src_platform_glfw_glfw.o: ./src/platform/glfw/glfw.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/platform/glfw/glfw.cpp -o src_platform_glfw_glfw.o

src_physics_physics.o: ./src/physics/physics.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/physics/physics.cpp -o src_physics_physics.o

src_physics_collisionmodel.o: ./src/physics/collisionmodel.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/physics/collisionmodel.cpp -o src_physics_collisionmodel.o

src_physics_bullet_bullet.o: ./src/physics/bullet/bullet.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/physics/bullet/bullet.cpp -o src_physics_bullet_bullet.o

src_framework_script.o: ./src/framework/script.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/script.cpp -o src_framework_script.o

src_framework_event.o: ./src/framework/event.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/event.cpp -o src_framework_event.o

src_framework_stats.o: ./src/framework/stats.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/stats.cpp -o src_framework_stats.o

src_framework_path.o: ./src/framework/path.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/path.cpp -o src_framework_path.o

src_framework_gui.o: ./src/framework/gui.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/gui.cpp -o src_framework_gui.o

src_framework_worldcell.o: ./src/framework/worldcell.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/worldcell.cpp -o src_framework_worldcell.o

src_framework_transition.o: ./src/framework/transition.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/transition.cpp -o src_framework_transition.o

src_framework_signal.o: ./src/framework/signal.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/signal.cpp -o src_framework_signal.o

src_framework_file.o: ./src/framework/file.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/file.cpp -o src_framework_file.o

src_framework_entity.o: ./src/framework/entity.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/entity.cpp -o src_framework_entity.o

src_framework_core.o: ./src/framework/core.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/core.cpp -o src_framework_core.o

src_framework_navigation.o: ./src/framework/navigation.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/navigation.cpp -o src_framework_navigation.o

src_framework_logging.o: ./src/framework/logging.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/logging.cpp -o src_framework_logging.o

src_framework_ui.o: ./src/framework/ui.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/ui.cpp -o src_framework_ui.o

src_framework_message.o: ./src/framework/message.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/message.cpp -o src_framework_message.o

src_framework_navmesh.o: ./src/framework/navmesh.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/navmesh.cpp -o src_framework_navmesh.o

src_framework_system.o: ./src/framework/system.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/system.cpp -o src_framework_system.o

src_framework_language.o: ./src/framework/language.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/language.cpp -o src_framework_language.o

src_framework_uid.o: ./src/framework/uid.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/uid.cpp -o src_framework_uid.o

src_framework_math.o: ./src/framework/math.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/math.cpp -o src_framework_math.o

src_framework_entitycomponent.o: ./src/framework/entitycomponent.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/entitycomponent.cpp -o src_framework_entitycomponent.o

src_framework_loader.o: ./src/framework/loader.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/loader.cpp -o src_framework_loader.o

src_framework_async.o: ./src/framework/async.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/framework/async.cpp -o src_framework_async.o

src_extensions_menu_menu.o: ./src/extensions/menu/menu.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/extensions/menu/menu.cpp -o src_extensions_menu_menu.o

src_extensions_camera_following.o: ./src/extensions/camera/following.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/extensions/camera/following.cpp -o src_extensions_camera_following.o

src_extensions_camera_camera.o: ./src/extensions/camera/camera.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/extensions/camera/camera.cpp -o src_extensions_camera_camera.o

src_extensions_camera_fpscamera.o: ./src/extensions/camera/fpscamera.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/extensions/camera/fpscamera.cpp -o src_extensions_camera_fpscamera.o

src_extensions_camera_firstperson.o: ./src/extensions/camera/firstperson.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/extensions/camera/firstperson.cpp -o src_extensions_camera_firstperson.o

src_extensions_camera_thirdperson.o: ./src/extensions/camera/thirdperson.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/extensions/camera/thirdperson.cpp -o src_extensions_camera_thirdperson.o

src_extensions_scripting_lua.o: ./src/extensions/scripting/lua.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/extensions/scripting/lua.cpp -o src_extensions_scripting_lua.o

src_extensions_kitchensink_soundtable.o: ./src/extensions/kitchensink/soundtable.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/extensions/kitchensink/soundtable.cpp -o src_extensions_kitchensink_soundtable.o

src_extensions_kitchensink_entities.o: ./src/extensions/kitchensink/entities.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/extensions/kitchensink/entities.cpp -o src_extensions_kitchensink_entities.o

src_extensions_kitchensink_attributes.o: ./src/extensions/kitchensink/attributes.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/extensions/kitchensink/attributes.cpp -o src_extensions_kitchensink_attributes.o

src_extensions_kitchensink_design.o: ./src/extensions/kitchensink/design.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/extensions/kitchensink/design.cpp -o src_extensions_kitchensink_design.o

src_extensions_kitchensink_character.o: ./src/extensions/kitchensink/character.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/extensions/kitchensink/character.cpp -o src_extensions_kitchensink_character.o

src_extensions_kitchensink_quest.o: ./src/extensions/kitchensink/quest.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/extensions/kitchensink/quest.cpp -o src_extensions_kitchensink_quest.o

src_extensions_kitchensink_dialog.o: ./src/extensions/kitchensink/dialog.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/extensions/kitchensink/dialog.cpp -o src_extensions_kitchensink_dialog.o

src_extensions_kitchensink_inventory.o: ./src/extensions/kitchensink/inventory.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/extensions/kitchensink/inventory.cpp -o src_extensions_kitchensink_inventory.o

src_audio_audio.o: ./src/audio/audio.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/audio/audio.cpp -o src_audio_audio.o

src_audio_sound.o: ./src/audio/sound.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/audio/sound.cpp -o src_audio_sound.o

src_audio_template_template.o: ./src/audio/template/template.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/audio/template/template.cpp -o src_audio_template_template.o

src_render_vertices.o: ./src/render/vertices.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/render/vertices.cpp -o src_render_vertices.o

src_render_renderer.o: ./src/render/renderer.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/render/renderer.cpp -o src_render_renderer.o

src_render_aabb.o: ./src/render/aabb.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/render/aabb.cpp -o src_render_aabb.o

src_render_error.o: ./src/render/error.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/render/error.cpp -o src_render_error.o

src_render_model.o: ./src/render/model.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/render/model.cpp -o src_render_model.o

src_render_render.o: ./src/render/render.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/render/render.cpp -o src_render_render.o

src_render_sprite.o: ./src/render/sprite.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/render/sprite.cpp -o src_render_sprite.o

src_render_animation.o: ./src/render/animation.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/render/animation.cpp -o src_render_animation.o

src_render_material.o: ./src/render/material.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/render/material.cpp -o src_render_material.o

src_render_opengl_renderer.o: ./src/render/opengl/renderer.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/render/opengl/renderer.cpp -o src_render_opengl_renderer.o

src_render_opengl_error.o: ./src/render/opengl/error.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/render/opengl/error.cpp -o src_render_opengl_error.o

src_render_opengl_shader.o: ./src/render/opengl/shader.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/render/opengl/shader.cpp -o src_render_opengl_shader.o

src_entities_staticworldobject.o: ./src/entities/staticworldobject.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/entities/staticworldobject.cpp -o src_entities_staticworldobject.o

src_entities_trigger.o: ./src/entities/trigger.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/entities/trigger.cpp -o src_entities_trigger.o

src_entities_player.o: ./src/entities/player.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/entities/player.cpp -o src_entities_player.o

src_entities_marker.o: ./src/entities/marker.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/entities/marker.cpp -o src_entities_marker.o

src_entities_crate.o: ./src/entities/crate.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/entities/crate.cpp -o src_entities_crate.o

src_entities_decoration.o: ./src/entities/decoration.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/entities/decoration.cpp -o src_entities_decoration.o

src_entities_sound.o: ./src/entities/sound.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/entities/sound.cpp -o src_entities_sound.o

src_entities_light.o: ./src/entities/light.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/entities/light.cpp -o src_entities_light.o

src_components_trigger.o: ./src/components/trigger.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/components/trigger.cpp -o src_components_trigger.o

src_components_player.o: ./src/components/player.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/components/player.cpp -o src_components_player.o

src_components_audio.o: ./src/components/audio.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/components/audio.cpp -o src_components_audio.o

src_components_physics.o: ./src/components/physics.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/components/physics.cpp -o src_components_physics.o

src_components_particle.o: ./src/components/particle.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/components/particle.cpp -o src_components_particle.o

src_components_render.o: ./src/components/render.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/components/render.cpp -o src_components_render.o

src_components_sprite.o: ./src/components/sprite.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/components/sprite.cpp -o src_components_sprite.o

src_components_animation.o: ./src/components/animation.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/components/animation.cpp -o src_components_animation.o

src_components_controller.o: ./src/components/controller.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/components/controller.cpp -o src_components_controller.o

src_components_light.o: ./src/components/light.cpp
	g++ -c -g -O0 -Wno-narrowing -std=c++20 -I./src -I./libraries -I/usr/include/bullet -I/usr/include/lua5.4 ./src/components/light.cpp -o src_components_light.o

clean:
	rm src_main.o
	rm src_platform_time.o
	rm src_platform_image.o
	rm src_platform_file.o
	rm src_platform_glfw_glfw.o
	rm src_physics_physics.o
	rm src_physics_collisionmodel.o
	rm src_physics_bullet_bullet.o
	rm src_framework_script.o
	rm src_framework_event.o
	rm src_framework_stats.o
	rm src_framework_path.o
	rm src_framework_gui.o
	rm src_framework_worldcell.o
	rm src_framework_transition.o
	rm src_framework_signal.o
	rm src_framework_file.o
	rm src_framework_entity.o
	rm src_framework_core.o
	rm src_framework_navigation.o
	rm src_framework_logging.o
	rm src_framework_ui.o
	rm src_framework_message.o
	rm src_framework_navmesh.o
	rm src_framework_system.o
	rm src_framework_language.o
	rm src_framework_uid.o
	rm src_framework_math.o
	rm src_framework_entitycomponent.o
	rm src_framework_loader.o
	rm src_framework_async.o
	rm src_extensions_menu_menu.o
	rm src_extensions_camera_following.o
	rm src_extensions_camera_camera.o
	rm src_extensions_camera_fpscamera.o
	rm src_extensions_camera_firstperson.o
	rm src_extensions_camera_thirdperson.o
	rm src_extensions_scripting_lua.o
	rm src_extensions_kitchensink_soundtable.o
	rm src_extensions_kitchensink_entities.o
	rm src_extensions_kitchensink_attributes.o
	rm src_extensions_kitchensink_design.o
	rm src_extensions_kitchensink_character.o
	rm src_extensions_kitchensink_quest.o
	rm src_extensions_kitchensink_dialog.o
	rm src_extensions_kitchensink_inventory.o
	rm src_audio_audio.o
	rm src_audio_sound.o
	rm src_audio_template_template.o
	rm src_render_vertices.o
	rm src_render_renderer.o
	rm src_render_aabb.o
	rm src_render_error.o
	rm src_render_model.o
	rm src_render_render.o
	rm src_render_sprite.o
	rm src_render_animation.o
	rm src_render_material.o
	rm src_render_opengl_renderer.o
	rm src_render_opengl_error.o
	rm src_render_opengl_shader.o
	rm src_entities_staticworldobject.o
	rm src_entities_trigger.o
	rm src_entities_player.o
	rm src_entities_marker.o
	rm src_entities_crate.o
	rm src_entities_decoration.o
	rm src_entities_sound.o
	rm src_entities_light.o
	rm src_components_trigger.o
	rm src_components_player.o
	rm src_components_audio.o
	rm src_components_physics.o
	rm src_components_particle.o
	rm src_components_render.o
	rm src_components_sprite.o
	rm src_components_animation.o
	rm src_components_controller.o
	rm src_components_light.o

project: src_main.o src_platform_time.o src_platform_image.o src_platform_file.o src_platform_glfw_glfw.o src_physics_physics.o src_physics_collisionmodel.o src_physics_bullet_bullet.o src_framework_script.o src_framework_event.o src_framework_stats.o src_framework_path.o src_framework_gui.o src_framework_worldcell.o src_framework_transition.o src_framework_signal.o src_framework_file.o src_framework_entity.o src_framework_core.o src_framework_navigation.o src_framework_logging.o src_framework_ui.o src_framework_message.o src_framework_navmesh.o src_framework_system.o src_framework_language.o src_framework_uid.o src_framework_math.o src_framework_entitycomponent.o src_framework_loader.o src_framework_async.o src_extensions_menu_menu.o src_extensions_camera_following.o src_extensions_camera_camera.o src_extensions_camera_fpscamera.o src_extensions_camera_firstperson.o src_extensions_camera_thirdperson.o src_extensions_scripting_lua.o src_extensions_kitchensink_soundtable.o src_extensions_kitchensink_entities.o src_extensions_kitchensink_attributes.o src_extensions_kitchensink_design.o src_extensions_kitchensink_character.o src_extensions_kitchensink_quest.o src_extensions_kitchensink_dialog.o src_extensions_kitchensink_inventory.o src_audio_audio.o src_audio_sound.o src_audio_template_template.o src_render_vertices.o src_render_renderer.o src_render_aabb.o src_render_error.o src_render_model.o src_render_render.o src_render_sprite.o src_render_animation.o src_render_material.o src_render_opengl_renderer.o src_render_opengl_error.o src_render_opengl_shader.o src_entities_staticworldobject.o src_entities_trigger.o src_entities_player.o src_entities_marker.o src_entities_crate.o src_entities_decoration.o src_entities_sound.o src_entities_light.o src_components_trigger.o src_components_player.o src_components_audio.o src_components_physics.o src_components_particle.o src_components_render.o src_components_sprite.o src_components_animation.o src_components_controller.o src_components_light.o 
	g++ -o tramsdk -L./ src_main.o src_platform_time.o src_platform_image.o src_platform_file.o src_platform_glfw_glfw.o src_physics_physics.o src_physics_collisionmodel.o src_physics_bullet_bullet.o src_framework_script.o src_framework_event.o src_framework_stats.o src_framework_path.o src_framework_gui.o src_framework_worldcell.o src_framework_transition.o src_framework_signal.o src_framework_file.o src_framework_entity.o src_framework_core.o src_framework_navigation.o src_framework_logging.o src_framework_ui.o src_framework_message.o src_framework_navmesh.o src_framework_system.o src_framework_language.o src_framework_uid.o src_framework_math.o src_framework_entitycomponent.o src_framework_loader.o src_framework_async.o src_extensions_menu_menu.o src_extensions_camera_following.o src_extensions_camera_camera.o src_extensions_camera_fpscamera.o src_extensions_camera_firstperson.o src_extensions_camera_thirdperson.o src_extensions_scripting_lua.o src_extensions_kitchensink_soundtable.o src_extensions_kitchensink_entities.o src_extensions_kitchensink_attributes.o src_extensions_kitchensink_design.o src_extensions_kitchensink_character.o src_extensions_kitchensink_quest.o src_extensions_kitchensink_dialog.o src_extensions_kitchensink_inventory.o src_audio_audio.o src_audio_sound.o src_audio_template_template.o src_render_vertices.o src_render_renderer.o src_render_aabb.o src_render_error.o src_render_model.o src_render_render.o src_render_sprite.o src_render_animation.o src_render_material.o src_render_opengl_renderer.o src_render_opengl_error.o src_render_opengl_shader.o src_entities_staticworldobject.o src_entities_trigger.o src_entities_player.o src_entities_marker.o src_entities_crate.o src_entities_decoration.o src_entities_sound.o src_entities_light.o src_components_trigger.o src_components_player.o src_components_audio.o src_components_physics.o src_components_particle.o src_components_render.o src_components_sprite.o src_components_animation.o src_components_controller.o src_components_light.o  -lglfw    -lGL  -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath       -llua5.4 