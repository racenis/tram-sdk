src_audio_audio.o: ./src/audio/audio.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/audio/audio.cpp -o src_audio_audio.o

src_audio_source.o: ./src/audio/source.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/audio/source.cpp -o src_audio_source.o

src_components_armaturecomponent.o: ./src/components/armaturecomponent.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/components/armaturecomponent.cpp -o src_components_armaturecomponent.o

src_components_controllercomponent.o: ./src/components/controllercomponent.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/components/controllercomponent.cpp -o src_components_controllercomponent.o

src_components_entitycomponents.o: ./src/components/entitycomponents.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/components/entitycomponents.cpp -o src_components_entitycomponents.o

src_components_particlecomponent.o: ./src/components/particlecomponent.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/components/particlecomponent.cpp -o src_components_particlecomponent.o

src_components_physicscomponent.o: ./src/components/physicscomponent.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/components/physicscomponent.cpp -o src_components_physicscomponent.o

src_components_rendercomponent.o: ./src/components/rendercomponent.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/components/rendercomponent.cpp -o src_components_rendercomponent.o

src_components_spritecomponent.o: ./src/components/spritecomponent.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/components/spritecomponent.cpp -o src_components_spritecomponent.o

src_components_triggercomponent.o: ./src/components/triggercomponent.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/components/triggercomponent.cpp -o src_components_triggercomponent.o

src_entities_bus.o: ./src/entities/bus.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/entities/bus.cpp -o src_entities_bus.o

src_entities_crate.o: ./src/entities/crate.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/entities/crate.cpp -o src_entities_crate.o

src_entities_lamp.o: ./src/entities/lamp.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/entities/lamp.cpp -o src_entities_lamp.o

src_entities_mongus.o: ./src/entities/mongus.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/entities/mongus.cpp -o src_entities_mongus.o

src_entities_player.o: ./src/entities/player.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/entities/player.cpp -o src_entities_player.o

src_entities_staticworldobject.o: ./src/entities/staticworldobject.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/entities/staticworldobject.cpp -o src_entities_staticworldobject.o

src_entities_tram.o: ./src/entities/tram.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/entities/tram.cpp -o src_entities_tram.o

src_extensions_camera_camera.o: ./src/extensions/camera/camera.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/extensions/camera/camera.cpp -o src_extensions_camera_camera.o

src_extensions_menu_menu.o: ./src/extensions/menu/menu.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/extensions/menu/menu.cpp -o src_extensions_menu_menu.o

src_framework_async.o: ./src/framework/async.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/async.cpp -o src_framework_async.o

src_framework_core.o: ./src/framework/core.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/core.cpp -o src_framework_core.o

src_framework_entity.o: ./src/framework/entity.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/entity.cpp -o src_framework_entity.o

src_framework_etc.o: ./src/framework/etc.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/etc.cpp -o src_framework_etc.o

src_framework_event.o: ./src/framework/event.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/event.cpp -o src_framework_event.o

src_framework_gui.o: ./src/framework/gui.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/gui.cpp -o src_framework_gui.o

src_framework_language.o: ./src/framework/language.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/language.cpp -o src_framework_language.o

src_framework_logging.o: ./src/framework/logging.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/logging.cpp -o src_framework_logging.o

src_framework_math.o: ./src/framework/math.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/math.cpp -o src_framework_math.o

src_framework_message.o: ./src/framework/message.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/message.cpp -o src_framework_message.o

src_framework_navigation.o: ./src/framework/navigation.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/navigation.cpp -o src_framework_navigation.o

src_framework_net.o: ./src/framework/net.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/net.cpp -o src_framework_net.o

src_framework_stats.o: ./src/framework/stats.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/stats.cpp -o src_framework_stats.o

src_framework_system.o: ./src/framework/system.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/system.cpp -o src_framework_system.o

src_framework_ui.o: ./src/framework/ui.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/ui.cpp -o src_framework_ui.o

src_framework_uid.o: ./src/framework/uid.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/uid.cpp -o src_framework_uid.o

src_framework_worldcell.o: ./src/framework/worldcell.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/framework/worldcell.cpp -o src_framework_worldcell.o

src_physics_collisionmodel.o: ./src/physics/collisionmodel.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/physics/collisionmodel.cpp -o src_physics_collisionmodel.o

src_physics_physics.o: ./src/physics/physics.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/physics/physics.cpp -o src_physics_physics.o

src_physics_bullet_bullet.o: ./src/physics/bullet/bullet.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/physics/bullet/bullet.cpp -o src_physics_bullet_bullet.o

src_platform_file.o: ./src/platform/file.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/platform/file.cpp -o src_platform_file.o

src_render_armature.o: ./src/render/armature.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/render/armature.cpp -o src_render_armature.o

src_render_material.o: ./src/render/material.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/render/material.cpp -o src_render_material.o

src_render_model.o: ./src/render/model.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/render/model.cpp -o src_render_model.o

src_render_render.o: ./src/render/render.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/render/render.cpp -o src_render_render.o

src_render_renderer.o: ./src/render/renderer.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/render/renderer.cpp -o src_render_renderer.o

src_render_render_misc.o: ./src/render/render_misc.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/render/render_misc.cpp -o src_render_render_misc.o

src_render_sprite.o: ./src/render/sprite.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/render/sprite.cpp -o src_render_sprite.o

src_render_opengl_renderer.o: ./src/render/opengl/renderer.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/render/opengl/renderer.cpp -o src_render_opengl_renderer.o

src_render_opengl_shader.o: ./src/render/opengl/shader.cpp
	g++ -c -O0 -std=c++20 -Ilibraries -I./src ./src/render/opengl/shader.cpp -o src_render_opengl_shader.o

clean:
	del src_audio_audio.o
	del src_audio_source.o
	del src_components_armaturecomponent.o
	del src_components_controllercomponent.o
	del src_components_entitycomponents.o
	del src_components_particlecomponent.o
	del src_components_physicscomponent.o
	del src_components_rendercomponent.o
	del src_components_spritecomponent.o
	del src_components_triggercomponent.o
	del src_entities_bus.o
	del src_entities_crate.o
	del src_entities_lamp.o
	del src_entities_mongus.o
	del src_entities_player.o
	del src_entities_staticworldobject.o
	del src_entities_tram.o
	del src_extensions_camera_camera.o
	del src_extensions_menu_menu.o
	del src_framework_async.o
	del src_framework_core.o
	del src_framework_entity.o
	del src_framework_etc.o
	del src_framework_event.o
	del src_framework_gui.o
	del src_framework_language.o
	del src_framework_logging.o
	del src_framework_math.o
	del src_framework_message.o
	del src_framework_navigation.o
	del src_framework_net.o
	del src_framework_stats.o
	del src_framework_system.o
	del src_framework_ui.o
	del src_framework_uid.o
	del src_framework_worldcell.o
	del src_physics_collisionmodel.o
	del src_physics_physics.o
	del src_physics_bullet_bullet.o
	del src_platform_file.o
	del src_render_armature.o
	del src_render_material.o
	del src_render_model.o
	del src_render_render.o
	del src_render_renderer.o
	del src_render_render_misc.o
	del src_render_sprite.o
	del src_render_opengl_renderer.o
	del src_render_opengl_shader.o

library: src_audio_audio.o src_audio_source.o src_components_armaturecomponent.o src_components_controllercomponent.o src_components_entitycomponents.o src_components_particlecomponent.o src_components_physicscomponent.o src_components_rendercomponent.o src_components_spritecomponent.o src_components_triggercomponent.o src_entities_bus.o src_entities_crate.o src_entities_lamp.o src_entities_mongus.o src_entities_player.o src_entities_staticworldobject.o src_entities_tram.o src_extensions_camera_camera.o src_extensions_menu_menu.o src_framework_async.o src_framework_core.o src_framework_entity.o src_framework_etc.o src_framework_event.o src_framework_gui.o src_framework_language.o src_framework_logging.o src_framework_math.o src_framework_message.o src_framework_navigation.o src_framework_net.o src_framework_stats.o src_framework_system.o src_framework_ui.o src_framework_uid.o src_framework_worldcell.o src_physics_collisionmodel.o src_physics_physics.o src_physics_bullet_bullet.o src_platform_file.o src_render_armature.o src_render_material.o src_render_model.o src_render_render.o src_render_renderer.o src_render_render_misc.o src_render_sprite.o src_render_opengl_renderer.o src_render_opengl_shader.o 
	ar -crf libtramsdk.a src_audio_audio.o src_audio_source.o src_components_armaturecomponent.o src_components_controllercomponent.o src_components_entitycomponents.o src_components_particlecomponent.o src_components_physicscomponent.o src_components_rendercomponent.o src_components_spritecomponent.o src_components_triggercomponent.o src_entities_bus.o src_entities_crate.o src_entities_lamp.o src_entities_mongus.o src_entities_player.o src_entities_staticworldobject.o src_entities_tram.o src_extensions_camera_camera.o src_extensions_menu_menu.o src_framework_async.o src_framework_core.o src_framework_entity.o src_framework_etc.o src_framework_event.o src_framework_gui.o src_framework_language.o src_framework_logging.o src_framework_math.o src_framework_message.o src_framework_navigation.o src_framework_net.o src_framework_stats.o src_framework_system.o src_framework_ui.o src_framework_uid.o src_framework_worldcell.o src_physics_collisionmodel.o src_physics_physics.o src_physics_bullet_bullet.o src_platform_file.o src_render_armature.o src_render_material.o src_render_model.o src_render_render.o src_render_renderer.o src_render_render_misc.o src_render_sprite.o src_render_opengl_renderer.o src_render_opengl_shader.o 
