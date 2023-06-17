src_audio_api.o: ./src/audio/api.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/audio/api.cpp -o src_audio_api.o

src_audio_audio.o: ./src/audio/audio.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/audio/audio.cpp -o src_audio_audio.o

src_audio_sound.o: ./src/audio/sound.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/audio/sound.cpp -o src_audio_sound.o

src_audio_openal_openal.o: ./src/audio/openal/openal.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/audio/openal/openal.cpp -o src_audio_openal_openal.o

src_components_animation.o: ./src/components/animation.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/components/animation.cpp -o src_components_animation.o

src_components_audio.o: ./src/components/audio.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/components/audio.cpp -o src_components_audio.o

src_components_controller.o: ./src/components/controller.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/components/controller.cpp -o src_components_controller.o

src_components_light.o: ./src/components/light.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/components/light.cpp -o src_components_light.o

src_components_particle.o: ./src/components/particle.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/components/particle.cpp -o src_components_particle.o

src_components_physics.o: ./src/components/physics.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/components/physics.cpp -o src_components_physics.o

src_components_player.o: ./src/components/player.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/components/player.cpp -o src_components_player.o

src_components_render.o: ./src/components/render.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/components/render.cpp -o src_components_render.o

src_components_sprite.o: ./src/components/sprite.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/components/sprite.cpp -o src_components_sprite.o

src_components_trigger.o: ./src/components/trigger.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/components/trigger.cpp -o src_components_trigger.o

src_entities_crate.o: ./src/entities/crate.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/entities/crate.cpp -o src_entities_crate.o

src_entities_lamp.o: ./src/entities/lamp.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/entities/lamp.cpp -o src_entities_lamp.o

src_entities_marker.o: ./src/entities/marker.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/entities/marker.cpp -o src_entities_marker.o

src_entities_player.o: ./src/entities/player.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/entities/player.cpp -o src_entities_player.o

src_entities_staticworldobject.o: ./src/entities/staticworldobject.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/entities/staticworldobject.cpp -o src_entities_staticworldobject.o

src_extensions_camera_camera.o: ./src/extensions/camera/camera.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/extensions/camera/camera.cpp -o src_extensions_camera_camera.o

src_extensions_menu_menu.o: ./src/extensions/menu/menu.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/extensions/menu/menu.cpp -o src_extensions_menu_menu.o

src_framework_async.o: ./src/framework/async.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/async.cpp -o src_framework_async.o

src_framework_core.o: ./src/framework/core.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/core.cpp -o src_framework_core.o

src_framework_entity.o: ./src/framework/entity.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/entity.cpp -o src_framework_entity.o

src_framework_entitycomponent.o: ./src/framework/entitycomponent.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/entitycomponent.cpp -o src_framework_entitycomponent.o

src_framework_event.o: ./src/framework/event.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/event.cpp -o src_framework_event.o

src_framework_file.o: ./src/framework/file.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/file.cpp -o src_framework_file.o

src_framework_gui.o: ./src/framework/gui.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/gui.cpp -o src_framework_gui.o

src_framework_language.o: ./src/framework/language.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/language.cpp -o src_framework_language.o

src_framework_loader.o: ./src/framework/loader.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/loader.cpp -o src_framework_loader.o

src_framework_logging.o: ./src/framework/logging.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/logging.cpp -o src_framework_logging.o

src_framework_math.o: ./src/framework/math.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/math.cpp -o src_framework_math.o

src_framework_message.o: ./src/framework/message.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/message.cpp -o src_framework_message.o

src_framework_navigation.o: ./src/framework/navigation.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/navigation.cpp -o src_framework_navigation.o

src_framework_navmesh.o: ./src/framework/navmesh.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/navmesh.cpp -o src_framework_navmesh.o

src_framework_path.o: ./src/framework/path.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/path.cpp -o src_framework_path.o

src_framework_stats.o: ./src/framework/stats.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/stats.cpp -o src_framework_stats.o

src_framework_system.o: ./src/framework/system.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/system.cpp -o src_framework_system.o

src_framework_transition.o: ./src/framework/transition.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/transition.cpp -o src_framework_transition.o

src_framework_ui.o: ./src/framework/ui.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/ui.cpp -o src_framework_ui.o

src_framework_uid.o: ./src/framework/uid.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/uid.cpp -o src_framework_uid.o

src_framework_worldcell.o: ./src/framework/worldcell.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/framework/worldcell.cpp -o src_framework_worldcell.o

src_physics_collisionmodel.o: ./src/physics/collisionmodel.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/physics/collisionmodel.cpp -o src_physics_collisionmodel.o

src_physics_physics.o: ./src/physics/physics.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/physics/physics.cpp -o src_physics_physics.o

src_physics_bullet_bullet.o: ./src/physics/bullet/bullet.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/physics/bullet/bullet.cpp -o src_physics_bullet_bullet.o

src_platform_file.o: ./src/platform/file.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/platform/file.cpp -o src_platform_file.o

src_platform_time.o: ./src/platform/time.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/platform/time.cpp -o src_platform_time.o

src_render_aabb.o: ./src/render/aabb.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/render/aabb.cpp -o src_render_aabb.o

src_render_animation.o: ./src/render/animation.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/render/animation.cpp -o src_render_animation.o

src_render_error.o: ./src/render/error.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/render/error.cpp -o src_render_error.o

src_render_material.o: ./src/render/material.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/render/material.cpp -o src_render_material.o

src_render_model.o: ./src/render/model.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/render/model.cpp -o src_render_model.o

src_render_render.o: ./src/render/render.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/render/render.cpp -o src_render_render.o

src_render_renderer.o: ./src/render/renderer.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/render/renderer.cpp -o src_render_renderer.o

src_render_sprite.o: ./src/render/sprite.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/render/sprite.cpp -o src_render_sprite.o

src_render_vertices.o: ./src/render/vertices.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/render/vertices.cpp -o src_render_vertices.o

src_render_opengl_error.o: ./src/render/opengl/error.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/render/opengl/error.cpp -o src_render_opengl_error.o

src_render_opengl_renderer.o: ./src/render/opengl/renderer.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/render/opengl/renderer.cpp -o src_render_opengl_renderer.o

src_render_opengl_shader.o: ./src/render/opengl/shader.cpp
	gcc -c -g -O0 -std=c++20 -I./src -Ilibraries -Ilibraries/bullet -Ilibraries/glad -Ilibraries/glfw3 -Ilibraries/openal ./src/render/opengl/shader.cpp -o src_render_opengl_shader.o

clean:
	del src_audio_api.o
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
	del src_entities_lamp.o
	del src_entities_marker.o
	del src_entities_player.o
	del src_entities_staticworldobject.o
	del src_extensions_camera_camera.o
	del src_extensions_menu_menu.o
	del src_framework_async.o
	del src_framework_core.o
	del src_framework_entity.o
	del src_framework_entitycomponent.o
	del src_framework_event.o
	del src_framework_file.o
	del src_framework_gui.o
	del src_framework_language.o
	del src_framework_loader.o
	del src_framework_logging.o
	del src_framework_math.o
	del src_framework_message.o
	del src_framework_navigation.o
	del src_framework_navmesh.o
	del src_framework_path.o
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
	del src_platform_time.o
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

library: src_audio_api.o src_audio_audio.o src_audio_sound.o src_audio_openal_openal.o src_components_animation.o src_components_audio.o src_components_controller.o src_components_light.o src_components_particle.o src_components_physics.o src_components_player.o src_components_render.o src_components_sprite.o src_components_trigger.o src_entities_crate.o src_entities_lamp.o src_entities_marker.o src_entities_player.o src_entities_staticworldobject.o src_extensions_camera_camera.o src_extensions_menu_menu.o src_framework_async.o src_framework_core.o src_framework_entity.o src_framework_entitycomponent.o src_framework_event.o src_framework_file.o src_framework_gui.o src_framework_language.o src_framework_loader.o src_framework_logging.o src_framework_math.o src_framework_message.o src_framework_navigation.o src_framework_navmesh.o src_framework_path.o src_framework_stats.o src_framework_system.o src_framework_transition.o src_framework_ui.o src_framework_uid.o src_framework_worldcell.o src_physics_collisionmodel.o src_physics_physics.o src_physics_bullet_bullet.o src_platform_file.o src_platform_time.o src_render_aabb.o src_render_animation.o src_render_error.o src_render_material.o src_render_model.o src_render_render.o src_render_renderer.o src_render_sprite.o src_render_vertices.o src_render_opengl_error.o src_render_opengl_renderer.o src_render_opengl_shader.o 
	ar -crf libtramsdk.a src_audio_api.o src_audio_audio.o src_audio_sound.o src_audio_openal_openal.o src_components_animation.o src_components_audio.o src_components_controller.o src_components_light.o src_components_particle.o src_components_physics.o src_components_player.o src_components_render.o src_components_sprite.o src_components_trigger.o src_entities_crate.o src_entities_lamp.o src_entities_marker.o src_entities_player.o src_entities_staticworldobject.o src_extensions_camera_camera.o src_extensions_menu_menu.o src_framework_async.o src_framework_core.o src_framework_entity.o src_framework_entitycomponent.o src_framework_event.o src_framework_file.o src_framework_gui.o src_framework_language.o src_framework_loader.o src_framework_logging.o src_framework_math.o src_framework_message.o src_framework_navigation.o src_framework_navmesh.o src_framework_path.o src_framework_stats.o src_framework_system.o src_framework_transition.o src_framework_ui.o src_framework_uid.o src_framework_worldcell.o src_physics_collisionmodel.o src_physics_physics.o src_physics_bullet_bullet.o src_platform_file.o src_platform_time.o src_render_aabb.o src_render_animation.o src_render_error.o src_render_material.o src_render_model.o src_render_render.o src_render_renderer.o src_render_sprite.o src_render_vertices.o src_render_opengl_error.o src_render_opengl_renderer.o src_render_opengl_shader.o 
