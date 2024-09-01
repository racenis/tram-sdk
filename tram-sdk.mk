##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=tram-sdk
ConfigurationName      :=Debug
WorkspaceConfiguration :=Debug
WorkspacePath          :=C:/Users/Poga/Desktop/painis/tram
ProjectPath            :=C:/Users/Poga/Desktop/painis/tram-sdk
IntermediateDirectory  :=../tram/build-$(WorkspaceConfiguration)/__/tram-sdk
OutDir                 :=$(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Poga
Date                   :=01/09/2024
CodeLitePath           :="C:/Program Files/CodeLite"
MakeDirCommand         :=mkdir
LinkerName             :=C:/mingw64/bin/g++.exe
SharedObjectLinkerName :=C:/mingw64/bin/g++.exe -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputDirectory        :=C:/Users/Poga/Desktop/painis/tram/build-$(WorkspaceConfiguration)/bin
OutputFile             :=..\tram\build-$(WorkspaceConfiguration)\bin\$(ProjectName).exe
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :=$(IntermediateDirectory)/ObjectsList.txt
PCHCompileFlags        :=
RcCmpOptions           := 
RcCompilerName         :=C:/mingw64/bin/windres.exe
LinkOptions            :=  -ld3dx9 -ld3d9 -lSDL2.dll -lglfw3 -lgdi32 -lopengl32 -lportaudio -lwinmm -lole32
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)./src $(IncludeSwitch)./libraries $(IncludeSwitch)./libraries/bullet $(IncludeSwitch)./libraries/glad $(IncludeSwitch)./libraries/lua $(IncludeSwitch)./libraries/openal $(IncludeSwitch)./libraries/sdl2 $(IncludeSwitch)./libraries/glfw 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)BulletSoftBody $(LibrarySwitch)BulletDynamics $(LibrarySwitch)BulletCollision $(LibrarySwitch)LinearMath $(LibrarySwitch)glfw3 $(LibrarySwitch)OpenAL32 $(LibrarySwitch)lua 
ArLibs                 :=  "libBulletSoftBody.a" "libBulletDynamics.a" "libBulletCollision.a" "libLinearMath.a" "libglfw3.a" "libOpenAL32.a" "liblua.a" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)libraries/binaries/win64 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := C:/mingw64/bin/ar.exe rcu
CXX      := C:/mingw64/bin/g++.exe
CC       := C:/mingw64/bin/gcc.exe
CXXFLAGS :=  -g -O0 -Wall -std=c++20 $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := C:/mingw64/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
WXWIN:=C:\Users\Poga\Desktop\miskaste\wxwidgets
WXCFG:=gcc_dll\mswu
Objects0=$(IntermediateDirectory)/src_extensions_menu_system.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_platform_glfw_glfw.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_audio_openal_openal.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_audio_spatial_spatial.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_menu_menu.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_menu_debug.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_scripting_lua.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_camera_firstperson.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_camera_thirdperson.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_extensions_kitchensink_attributes.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_kitchensink_character.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_kitchensink_design.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_camera_following.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_kitchensink_dialog.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_kitchensink_entities.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_kitchensink_quest.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_kitchensink_soundtable.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_camera_camera.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_crate.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_entities_player.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_light.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_decoration.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_audio_spatial_output.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_trigger.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_ui.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_transition.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_loader.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_staticworldobject.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_vertices.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_render_opengl_renderer.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_language.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_graph.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_script.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_render.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_entity.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_animation.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_audio_audio.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_opengl_shader.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_system.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_components_sprite.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_physics.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_file.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_light.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_entitycomponent.cpp$(ObjectSuffix) 

Objects1=$(IntermediateDirectory)/src_components_render.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_audio.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_player.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_renderer.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_kitchensink_inventory.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_audio_sound.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_navmesh.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_settings.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_controller.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_audio_spatial_pathtracer.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_particle.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_message.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_marker.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_gui.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_framework_core.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_platform_time.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_sound.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_navigation.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_async.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_worldcell.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_uid.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_stats.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_event.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_material.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_framework_path.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_math.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_platform_image.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_platform_file.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_physics_physics.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_animation.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_physics_collisionmodel.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_cli.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_trigger.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_aabb.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_render_error.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_signal.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_model.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_logging.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_sprite.cpp$(ObjectSuffix) 



Objects=$(Objects0) $(Objects1) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: MakeIntermediateDirs $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	@echo $(Objects1) >> $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"
	@if not exist "$(OutputDirectory)" $(MakeDirCommand) "$(OutputDirectory)"

$(IntermediateDirectory)/.d:
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_extensions_menu_system.cpp$(ObjectSuffix): src/extensions/menu/system.cpp $(IntermediateDirectory)/src_extensions_menu_system.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/menu/system.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_menu_system.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_menu_system.cpp$(DependSuffix): src/extensions/menu/system.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_menu_system.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_menu_system.cpp$(DependSuffix) -MM src/extensions/menu/system.cpp

$(IntermediateDirectory)/src_extensions_menu_system.cpp$(PreprocessSuffix): src/extensions/menu/system.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_menu_system.cpp$(PreprocessSuffix) src/extensions/menu/system.cpp

$(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(ObjectSuffix): src/physics/bullet/bullet.cpp $(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/physics/bullet/bullet.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(DependSuffix): src/physics/bullet/bullet.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(DependSuffix) -MM src/physics/bullet/bullet.cpp

$(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(PreprocessSuffix): src/physics/bullet/bullet.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(PreprocessSuffix) src/physics/bullet/bullet.cpp

$(IntermediateDirectory)/src_platform_glfw_glfw.cpp$(ObjectSuffix): src/platform/glfw/glfw.cpp $(IntermediateDirectory)/src_platform_glfw_glfw.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/platform/glfw/glfw.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_platform_glfw_glfw.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_platform_glfw_glfw.cpp$(DependSuffix): src/platform/glfw/glfw.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_platform_glfw_glfw.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_platform_glfw_glfw.cpp$(DependSuffix) -MM src/platform/glfw/glfw.cpp

$(IntermediateDirectory)/src_platform_glfw_glfw.cpp$(PreprocessSuffix): src/platform/glfw/glfw.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_platform_glfw_glfw.cpp$(PreprocessSuffix) src/platform/glfw/glfw.cpp

$(IntermediateDirectory)/src_audio_openal_openal.cpp$(ObjectSuffix): src/audio/openal/openal.cpp $(IntermediateDirectory)/src_audio_openal_openal.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/audio/openal/openal.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_audio_openal_openal.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_audio_openal_openal.cpp$(DependSuffix): src/audio/openal/openal.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_audio_openal_openal.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_audio_openal_openal.cpp$(DependSuffix) -MM src/audio/openal/openal.cpp

$(IntermediateDirectory)/src_audio_openal_openal.cpp$(PreprocessSuffix): src/audio/openal/openal.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_audio_openal_openal.cpp$(PreprocessSuffix) src/audio/openal/openal.cpp

$(IntermediateDirectory)/src_audio_spatial_spatial.cpp$(ObjectSuffix): src/audio/spatial/spatial.cpp $(IntermediateDirectory)/src_audio_spatial_spatial.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/audio/spatial/spatial.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_audio_spatial_spatial.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_audio_spatial_spatial.cpp$(DependSuffix): src/audio/spatial/spatial.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_audio_spatial_spatial.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_audio_spatial_spatial.cpp$(DependSuffix) -MM src/audio/spatial/spatial.cpp

$(IntermediateDirectory)/src_audio_spatial_spatial.cpp$(PreprocessSuffix): src/audio/spatial/spatial.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_audio_spatial_spatial.cpp$(PreprocessSuffix) src/audio/spatial/spatial.cpp

$(IntermediateDirectory)/src_extensions_menu_menu.cpp$(ObjectSuffix): src/extensions/menu/menu.cpp $(IntermediateDirectory)/src_extensions_menu_menu.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/menu/menu.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_menu_menu.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_menu_menu.cpp$(DependSuffix): src/extensions/menu/menu.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_menu_menu.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_menu_menu.cpp$(DependSuffix) -MM src/extensions/menu/menu.cpp

$(IntermediateDirectory)/src_extensions_menu_menu.cpp$(PreprocessSuffix): src/extensions/menu/menu.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_menu_menu.cpp$(PreprocessSuffix) src/extensions/menu/menu.cpp

$(IntermediateDirectory)/src_extensions_menu_debug.cpp$(ObjectSuffix): src/extensions/menu/debug.cpp $(IntermediateDirectory)/src_extensions_menu_debug.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/menu/debug.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_menu_debug.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_menu_debug.cpp$(DependSuffix): src/extensions/menu/debug.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_menu_debug.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_menu_debug.cpp$(DependSuffix) -MM src/extensions/menu/debug.cpp

$(IntermediateDirectory)/src_extensions_menu_debug.cpp$(PreprocessSuffix): src/extensions/menu/debug.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_menu_debug.cpp$(PreprocessSuffix) src/extensions/menu/debug.cpp

$(IntermediateDirectory)/src_extensions_scripting_lua.cpp$(ObjectSuffix): src/extensions/scripting/lua.cpp $(IntermediateDirectory)/src_extensions_scripting_lua.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/scripting/lua.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_scripting_lua.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_scripting_lua.cpp$(DependSuffix): src/extensions/scripting/lua.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_scripting_lua.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_scripting_lua.cpp$(DependSuffix) -MM src/extensions/scripting/lua.cpp

$(IntermediateDirectory)/src_extensions_scripting_lua.cpp$(PreprocessSuffix): src/extensions/scripting/lua.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_scripting_lua.cpp$(PreprocessSuffix) src/extensions/scripting/lua.cpp

$(IntermediateDirectory)/src_extensions_camera_firstperson.cpp$(ObjectSuffix): src/extensions/camera/firstperson.cpp $(IntermediateDirectory)/src_extensions_camera_firstperson.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/camera/firstperson.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_camera_firstperson.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_camera_firstperson.cpp$(DependSuffix): src/extensions/camera/firstperson.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_camera_firstperson.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_camera_firstperson.cpp$(DependSuffix) -MM src/extensions/camera/firstperson.cpp

$(IntermediateDirectory)/src_extensions_camera_firstperson.cpp$(PreprocessSuffix): src/extensions/camera/firstperson.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_camera_firstperson.cpp$(PreprocessSuffix) src/extensions/camera/firstperson.cpp

$(IntermediateDirectory)/src_extensions_camera_thirdperson.cpp$(ObjectSuffix): src/extensions/camera/thirdperson.cpp $(IntermediateDirectory)/src_extensions_camera_thirdperson.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/camera/thirdperson.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_camera_thirdperson.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_camera_thirdperson.cpp$(DependSuffix): src/extensions/camera/thirdperson.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_camera_thirdperson.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_camera_thirdperson.cpp$(DependSuffix) -MM src/extensions/camera/thirdperson.cpp

$(IntermediateDirectory)/src_extensions_camera_thirdperson.cpp$(PreprocessSuffix): src/extensions/camera/thirdperson.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_camera_thirdperson.cpp$(PreprocessSuffix) src/extensions/camera/thirdperson.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_attributes.cpp$(ObjectSuffix): src/extensions/kitchensink/attributes.cpp $(IntermediateDirectory)/src_extensions_kitchensink_attributes.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/kitchensink/attributes.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_kitchensink_attributes.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_kitchensink_attributes.cpp$(DependSuffix): src/extensions/kitchensink/attributes.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_kitchensink_attributes.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_kitchensink_attributes.cpp$(DependSuffix) -MM src/extensions/kitchensink/attributes.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_attributes.cpp$(PreprocessSuffix): src/extensions/kitchensink/attributes.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_kitchensink_attributes.cpp$(PreprocessSuffix) src/extensions/kitchensink/attributes.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_character.cpp$(ObjectSuffix): src/extensions/kitchensink/character.cpp $(IntermediateDirectory)/src_extensions_kitchensink_character.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/kitchensink/character.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_kitchensink_character.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_kitchensink_character.cpp$(DependSuffix): src/extensions/kitchensink/character.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_kitchensink_character.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_kitchensink_character.cpp$(DependSuffix) -MM src/extensions/kitchensink/character.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_character.cpp$(PreprocessSuffix): src/extensions/kitchensink/character.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_kitchensink_character.cpp$(PreprocessSuffix) src/extensions/kitchensink/character.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_design.cpp$(ObjectSuffix): src/extensions/kitchensink/design.cpp $(IntermediateDirectory)/src_extensions_kitchensink_design.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/kitchensink/design.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_kitchensink_design.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_kitchensink_design.cpp$(DependSuffix): src/extensions/kitchensink/design.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_kitchensink_design.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_kitchensink_design.cpp$(DependSuffix) -MM src/extensions/kitchensink/design.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_design.cpp$(PreprocessSuffix): src/extensions/kitchensink/design.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_kitchensink_design.cpp$(PreprocessSuffix) src/extensions/kitchensink/design.cpp

$(IntermediateDirectory)/src_extensions_camera_following.cpp$(ObjectSuffix): src/extensions/camera/following.cpp $(IntermediateDirectory)/src_extensions_camera_following.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/camera/following.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_camera_following.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_camera_following.cpp$(DependSuffix): src/extensions/camera/following.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_camera_following.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_camera_following.cpp$(DependSuffix) -MM src/extensions/camera/following.cpp

$(IntermediateDirectory)/src_extensions_camera_following.cpp$(PreprocessSuffix): src/extensions/camera/following.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_camera_following.cpp$(PreprocessSuffix) src/extensions/camera/following.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_dialog.cpp$(ObjectSuffix): src/extensions/kitchensink/dialog.cpp $(IntermediateDirectory)/src_extensions_kitchensink_dialog.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/kitchensink/dialog.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_kitchensink_dialog.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_kitchensink_dialog.cpp$(DependSuffix): src/extensions/kitchensink/dialog.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_kitchensink_dialog.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_kitchensink_dialog.cpp$(DependSuffix) -MM src/extensions/kitchensink/dialog.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_dialog.cpp$(PreprocessSuffix): src/extensions/kitchensink/dialog.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_kitchensink_dialog.cpp$(PreprocessSuffix) src/extensions/kitchensink/dialog.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_entities.cpp$(ObjectSuffix): src/extensions/kitchensink/entities.cpp $(IntermediateDirectory)/src_extensions_kitchensink_entities.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/kitchensink/entities.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_kitchensink_entities.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_kitchensink_entities.cpp$(DependSuffix): src/extensions/kitchensink/entities.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_kitchensink_entities.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_kitchensink_entities.cpp$(DependSuffix) -MM src/extensions/kitchensink/entities.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_entities.cpp$(PreprocessSuffix): src/extensions/kitchensink/entities.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_kitchensink_entities.cpp$(PreprocessSuffix) src/extensions/kitchensink/entities.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_quest.cpp$(ObjectSuffix): src/extensions/kitchensink/quest.cpp $(IntermediateDirectory)/src_extensions_kitchensink_quest.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/kitchensink/quest.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_kitchensink_quest.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_kitchensink_quest.cpp$(DependSuffix): src/extensions/kitchensink/quest.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_kitchensink_quest.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_kitchensink_quest.cpp$(DependSuffix) -MM src/extensions/kitchensink/quest.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_quest.cpp$(PreprocessSuffix): src/extensions/kitchensink/quest.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_kitchensink_quest.cpp$(PreprocessSuffix) src/extensions/kitchensink/quest.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_soundtable.cpp$(ObjectSuffix): src/extensions/kitchensink/soundtable.cpp $(IntermediateDirectory)/src_extensions_kitchensink_soundtable.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/kitchensink/soundtable.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_kitchensink_soundtable.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_kitchensink_soundtable.cpp$(DependSuffix): src/extensions/kitchensink/soundtable.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_kitchensink_soundtable.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_kitchensink_soundtable.cpp$(DependSuffix) -MM src/extensions/kitchensink/soundtable.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_soundtable.cpp$(PreprocessSuffix): src/extensions/kitchensink/soundtable.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_kitchensink_soundtable.cpp$(PreprocessSuffix) src/extensions/kitchensink/soundtable.cpp

$(IntermediateDirectory)/src_extensions_camera_camera.cpp$(ObjectSuffix): src/extensions/camera/camera.cpp $(IntermediateDirectory)/src_extensions_camera_camera.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/camera/camera.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_camera_camera.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_camera_camera.cpp$(DependSuffix): src/extensions/camera/camera.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_camera_camera.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_camera_camera.cpp$(DependSuffix) -MM src/extensions/camera/camera.cpp

$(IntermediateDirectory)/src_extensions_camera_camera.cpp$(PreprocessSuffix): src/extensions/camera/camera.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_camera_camera.cpp$(PreprocessSuffix) src/extensions/camera/camera.cpp

$(IntermediateDirectory)/src_entities_crate.cpp$(ObjectSuffix): src/entities/crate.cpp $(IntermediateDirectory)/src_entities_crate.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/crate.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_crate.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_crate.cpp$(DependSuffix): src/entities/crate.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_crate.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_crate.cpp$(DependSuffix) -MM src/entities/crate.cpp

$(IntermediateDirectory)/src_entities_crate.cpp$(PreprocessSuffix): src/entities/crate.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_crate.cpp$(PreprocessSuffix) src/entities/crate.cpp

$(IntermediateDirectory)/src_entities_player.cpp$(ObjectSuffix): src/entities/player.cpp $(IntermediateDirectory)/src_entities_player.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/player.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_player.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_player.cpp$(DependSuffix): src/entities/player.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_player.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_player.cpp$(DependSuffix) -MM src/entities/player.cpp

$(IntermediateDirectory)/src_entities_player.cpp$(PreprocessSuffix): src/entities/player.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_player.cpp$(PreprocessSuffix) src/entities/player.cpp

$(IntermediateDirectory)/src_entities_light.cpp$(ObjectSuffix): src/entities/light.cpp $(IntermediateDirectory)/src_entities_light.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/light.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_light.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_light.cpp$(DependSuffix): src/entities/light.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_light.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_light.cpp$(DependSuffix) -MM src/entities/light.cpp

$(IntermediateDirectory)/src_entities_light.cpp$(PreprocessSuffix): src/entities/light.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_light.cpp$(PreprocessSuffix) src/entities/light.cpp

$(IntermediateDirectory)/src_entities_decoration.cpp$(ObjectSuffix): src/entities/decoration.cpp $(IntermediateDirectory)/src_entities_decoration.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/decoration.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_decoration.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_decoration.cpp$(DependSuffix): src/entities/decoration.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_decoration.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_decoration.cpp$(DependSuffix) -MM src/entities/decoration.cpp

$(IntermediateDirectory)/src_entities_decoration.cpp$(PreprocessSuffix): src/entities/decoration.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_decoration.cpp$(PreprocessSuffix) src/entities/decoration.cpp

$(IntermediateDirectory)/src_audio_spatial_output.cpp$(ObjectSuffix): src/audio/spatial/output.cpp $(IntermediateDirectory)/src_audio_spatial_output.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/audio/spatial/output.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_audio_spatial_output.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_audio_spatial_output.cpp$(DependSuffix): src/audio/spatial/output.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_audio_spatial_output.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_audio_spatial_output.cpp$(DependSuffix) -MM src/audio/spatial/output.cpp

$(IntermediateDirectory)/src_audio_spatial_output.cpp$(PreprocessSuffix): src/audio/spatial/output.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_audio_spatial_output.cpp$(PreprocessSuffix) src/audio/spatial/output.cpp

$(IntermediateDirectory)/src_entities_trigger.cpp$(ObjectSuffix): src/entities/trigger.cpp $(IntermediateDirectory)/src_entities_trigger.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/trigger.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_trigger.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_trigger.cpp$(DependSuffix): src/entities/trigger.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_trigger.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_trigger.cpp$(DependSuffix) -MM src/entities/trigger.cpp

$(IntermediateDirectory)/src_entities_trigger.cpp$(PreprocessSuffix): src/entities/trigger.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_trigger.cpp$(PreprocessSuffix) src/entities/trigger.cpp

$(IntermediateDirectory)/src_framework_ui.cpp$(ObjectSuffix): src/framework/ui.cpp $(IntermediateDirectory)/src_framework_ui.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/ui.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_ui.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_ui.cpp$(DependSuffix): src/framework/ui.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_ui.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_ui.cpp$(DependSuffix) -MM src/framework/ui.cpp

$(IntermediateDirectory)/src_framework_ui.cpp$(PreprocessSuffix): src/framework/ui.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_ui.cpp$(PreprocessSuffix) src/framework/ui.cpp

$(IntermediateDirectory)/src_framework_transition.cpp$(ObjectSuffix): src/framework/transition.cpp $(IntermediateDirectory)/src_framework_transition.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/transition.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_transition.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_transition.cpp$(DependSuffix): src/framework/transition.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_transition.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_transition.cpp$(DependSuffix) -MM src/framework/transition.cpp

$(IntermediateDirectory)/src_framework_transition.cpp$(PreprocessSuffix): src/framework/transition.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_transition.cpp$(PreprocessSuffix) src/framework/transition.cpp

$(IntermediateDirectory)/src_framework_loader.cpp$(ObjectSuffix): src/framework/loader.cpp $(IntermediateDirectory)/src_framework_loader.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/loader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_loader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_loader.cpp$(DependSuffix): src/framework/loader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_loader.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_loader.cpp$(DependSuffix) -MM src/framework/loader.cpp

$(IntermediateDirectory)/src_framework_loader.cpp$(PreprocessSuffix): src/framework/loader.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_loader.cpp$(PreprocessSuffix) src/framework/loader.cpp

$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(ObjectSuffix): src/entities/staticworldobject.cpp $(IntermediateDirectory)/src_entities_staticworldobject.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/staticworldobject.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(DependSuffix): src/entities/staticworldobject.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(DependSuffix) -MM src/entities/staticworldobject.cpp

$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(PreprocessSuffix): src/entities/staticworldobject.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_staticworldobject.cpp$(PreprocessSuffix) src/entities/staticworldobject.cpp

$(IntermediateDirectory)/src_render_vertices.cpp$(ObjectSuffix): src/render/vertices.cpp $(IntermediateDirectory)/src_render_vertices.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/vertices.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_vertices.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_vertices.cpp$(DependSuffix): src/render/vertices.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_vertices.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_vertices.cpp$(DependSuffix) -MM src/render/vertices.cpp

$(IntermediateDirectory)/src_render_vertices.cpp$(PreprocessSuffix): src/render/vertices.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_vertices.cpp$(PreprocessSuffix) src/render/vertices.cpp

$(IntermediateDirectory)/src_render_opengl_renderer.cpp$(ObjectSuffix): src/render/opengl/renderer.cpp $(IntermediateDirectory)/src_render_opengl_renderer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/opengl/renderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_opengl_renderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_opengl_renderer.cpp$(DependSuffix): src/render/opengl/renderer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_opengl_renderer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_opengl_renderer.cpp$(DependSuffix) -MM src/render/opengl/renderer.cpp

$(IntermediateDirectory)/src_render_opengl_renderer.cpp$(PreprocessSuffix): src/render/opengl/renderer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_opengl_renderer.cpp$(PreprocessSuffix) src/render/opengl/renderer.cpp

$(IntermediateDirectory)/src_framework_language.cpp$(ObjectSuffix): src/framework/language.cpp $(IntermediateDirectory)/src_framework_language.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/language.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_language.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_language.cpp$(DependSuffix): src/framework/language.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_language.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_language.cpp$(DependSuffix) -MM src/framework/language.cpp

$(IntermediateDirectory)/src_framework_language.cpp$(PreprocessSuffix): src/framework/language.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_language.cpp$(PreprocessSuffix) src/framework/language.cpp

$(IntermediateDirectory)/src_framework_graph.cpp$(ObjectSuffix): src/framework/graph.cpp $(IntermediateDirectory)/src_framework_graph.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/graph.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_graph.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_graph.cpp$(DependSuffix): src/framework/graph.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_graph.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_graph.cpp$(DependSuffix) -MM src/framework/graph.cpp

$(IntermediateDirectory)/src_framework_graph.cpp$(PreprocessSuffix): src/framework/graph.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_graph.cpp$(PreprocessSuffix) src/framework/graph.cpp

$(IntermediateDirectory)/src_framework_script.cpp$(ObjectSuffix): src/framework/script.cpp $(IntermediateDirectory)/src_framework_script.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/script.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_script.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_script.cpp$(DependSuffix): src/framework/script.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_script.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_script.cpp$(DependSuffix) -MM src/framework/script.cpp

$(IntermediateDirectory)/src_framework_script.cpp$(PreprocessSuffix): src/framework/script.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_script.cpp$(PreprocessSuffix) src/framework/script.cpp

$(IntermediateDirectory)/src_render_render.cpp$(ObjectSuffix): src/render/render.cpp $(IntermediateDirectory)/src_render_render.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/render.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_render.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_render.cpp$(DependSuffix): src/render/render.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_render.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_render.cpp$(DependSuffix) -MM src/render/render.cpp

$(IntermediateDirectory)/src_render_render.cpp$(PreprocessSuffix): src/render/render.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_render.cpp$(PreprocessSuffix) src/render/render.cpp

$(IntermediateDirectory)/src_framework_entity.cpp$(ObjectSuffix): src/framework/entity.cpp $(IntermediateDirectory)/src_framework_entity.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/entity.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_entity.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_entity.cpp$(DependSuffix): src/framework/entity.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_entity.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_entity.cpp$(DependSuffix) -MM src/framework/entity.cpp

$(IntermediateDirectory)/src_framework_entity.cpp$(PreprocessSuffix): src/framework/entity.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_entity.cpp$(PreprocessSuffix) src/framework/entity.cpp

$(IntermediateDirectory)/src_components_animation.cpp$(ObjectSuffix): src/components/animation.cpp $(IntermediateDirectory)/src_components_animation.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/animation.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_animation.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_animation.cpp$(DependSuffix): src/components/animation.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_animation.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_animation.cpp$(DependSuffix) -MM src/components/animation.cpp

$(IntermediateDirectory)/src_components_animation.cpp$(PreprocessSuffix): src/components/animation.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_animation.cpp$(PreprocessSuffix) src/components/animation.cpp

$(IntermediateDirectory)/src_audio_audio.cpp$(ObjectSuffix): src/audio/audio.cpp $(IntermediateDirectory)/src_audio_audio.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/audio/audio.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_audio_audio.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_audio_audio.cpp$(DependSuffix): src/audio/audio.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_audio_audio.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_audio_audio.cpp$(DependSuffix) -MM src/audio/audio.cpp

$(IntermediateDirectory)/src_audio_audio.cpp$(PreprocessSuffix): src/audio/audio.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_audio_audio.cpp$(PreprocessSuffix) src/audio/audio.cpp

$(IntermediateDirectory)/src_render_opengl_shader.cpp$(ObjectSuffix): src/render/opengl/shader.cpp $(IntermediateDirectory)/src_render_opengl_shader.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/opengl/shader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_opengl_shader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_opengl_shader.cpp$(DependSuffix): src/render/opengl/shader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_opengl_shader.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_opengl_shader.cpp$(DependSuffix) -MM src/render/opengl/shader.cpp

$(IntermediateDirectory)/src_render_opengl_shader.cpp$(PreprocessSuffix): src/render/opengl/shader.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_opengl_shader.cpp$(PreprocessSuffix) src/render/opengl/shader.cpp

$(IntermediateDirectory)/src_framework_system.cpp$(ObjectSuffix): src/framework/system.cpp $(IntermediateDirectory)/src_framework_system.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/system.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_system.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_system.cpp$(DependSuffix): src/framework/system.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_system.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_system.cpp$(DependSuffix) -MM src/framework/system.cpp

$(IntermediateDirectory)/src_framework_system.cpp$(PreprocessSuffix): src/framework/system.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_system.cpp$(PreprocessSuffix) src/framework/system.cpp

$(IntermediateDirectory)/src_components_sprite.cpp$(ObjectSuffix): src/components/sprite.cpp $(IntermediateDirectory)/src_components_sprite.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/sprite.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_sprite.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_sprite.cpp$(DependSuffix): src/components/sprite.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_sprite.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_sprite.cpp$(DependSuffix) -MM src/components/sprite.cpp

$(IntermediateDirectory)/src_components_sprite.cpp$(PreprocessSuffix): src/components/sprite.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_sprite.cpp$(PreprocessSuffix) src/components/sprite.cpp

$(IntermediateDirectory)/src_components_physics.cpp$(ObjectSuffix): src/components/physics.cpp $(IntermediateDirectory)/src_components_physics.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/physics.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_physics.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_physics.cpp$(DependSuffix): src/components/physics.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_physics.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_physics.cpp$(DependSuffix) -MM src/components/physics.cpp

$(IntermediateDirectory)/src_components_physics.cpp$(PreprocessSuffix): src/components/physics.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_physics.cpp$(PreprocessSuffix) src/components/physics.cpp

$(IntermediateDirectory)/src_framework_file.cpp$(ObjectSuffix): src/framework/file.cpp $(IntermediateDirectory)/src_framework_file.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/file.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_file.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_file.cpp$(DependSuffix): src/framework/file.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_file.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_file.cpp$(DependSuffix) -MM src/framework/file.cpp

$(IntermediateDirectory)/src_framework_file.cpp$(PreprocessSuffix): src/framework/file.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_file.cpp$(PreprocessSuffix) src/framework/file.cpp

$(IntermediateDirectory)/src_components_light.cpp$(ObjectSuffix): src/components/light.cpp $(IntermediateDirectory)/src_components_light.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/light.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_light.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_light.cpp$(DependSuffix): src/components/light.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_light.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_light.cpp$(DependSuffix) -MM src/components/light.cpp

$(IntermediateDirectory)/src_components_light.cpp$(PreprocessSuffix): src/components/light.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_light.cpp$(PreprocessSuffix) src/components/light.cpp

$(IntermediateDirectory)/src_framework_entitycomponent.cpp$(ObjectSuffix): src/framework/entitycomponent.cpp $(IntermediateDirectory)/src_framework_entitycomponent.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/entitycomponent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_entitycomponent.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_entitycomponent.cpp$(DependSuffix): src/framework/entitycomponent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_entitycomponent.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_entitycomponent.cpp$(DependSuffix) -MM src/framework/entitycomponent.cpp

$(IntermediateDirectory)/src_framework_entitycomponent.cpp$(PreprocessSuffix): src/framework/entitycomponent.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_entitycomponent.cpp$(PreprocessSuffix) src/framework/entitycomponent.cpp

$(IntermediateDirectory)/src_components_render.cpp$(ObjectSuffix): src/components/render.cpp $(IntermediateDirectory)/src_components_render.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/render.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_render.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_render.cpp$(DependSuffix): src/components/render.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_render.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_render.cpp$(DependSuffix) -MM src/components/render.cpp

$(IntermediateDirectory)/src_components_render.cpp$(PreprocessSuffix): src/components/render.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_render.cpp$(PreprocessSuffix) src/components/render.cpp

$(IntermediateDirectory)/src_components_audio.cpp$(ObjectSuffix): src/components/audio.cpp $(IntermediateDirectory)/src_components_audio.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/audio.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_audio.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_audio.cpp$(DependSuffix): src/components/audio.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_audio.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_audio.cpp$(DependSuffix) -MM src/components/audio.cpp

$(IntermediateDirectory)/src_components_audio.cpp$(PreprocessSuffix): src/components/audio.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_audio.cpp$(PreprocessSuffix) src/components/audio.cpp

$(IntermediateDirectory)/src_components_player.cpp$(ObjectSuffix): src/components/player.cpp $(IntermediateDirectory)/src_components_player.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/player.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_player.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_player.cpp$(DependSuffix): src/components/player.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_player.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_player.cpp$(DependSuffix) -MM src/components/player.cpp

$(IntermediateDirectory)/src_components_player.cpp$(PreprocessSuffix): src/components/player.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_player.cpp$(PreprocessSuffix) src/components/player.cpp

$(IntermediateDirectory)/src_render_renderer.cpp$(ObjectSuffix): src/render/renderer.cpp $(IntermediateDirectory)/src_render_renderer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/renderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_renderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_renderer.cpp$(DependSuffix): src/render/renderer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_renderer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_renderer.cpp$(DependSuffix) -MM src/render/renderer.cpp

$(IntermediateDirectory)/src_render_renderer.cpp$(PreprocessSuffix): src/render/renderer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_renderer.cpp$(PreprocessSuffix) src/render/renderer.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_inventory.cpp$(ObjectSuffix): src/extensions/kitchensink/inventory.cpp $(IntermediateDirectory)/src_extensions_kitchensink_inventory.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/kitchensink/inventory.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_kitchensink_inventory.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_kitchensink_inventory.cpp$(DependSuffix): src/extensions/kitchensink/inventory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_kitchensink_inventory.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_kitchensink_inventory.cpp$(DependSuffix) -MM src/extensions/kitchensink/inventory.cpp

$(IntermediateDirectory)/src_extensions_kitchensink_inventory.cpp$(PreprocessSuffix): src/extensions/kitchensink/inventory.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_kitchensink_inventory.cpp$(PreprocessSuffix) src/extensions/kitchensink/inventory.cpp

$(IntermediateDirectory)/src_audio_sound.cpp$(ObjectSuffix): src/audio/sound.cpp $(IntermediateDirectory)/src_audio_sound.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/audio/sound.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_audio_sound.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_audio_sound.cpp$(DependSuffix): src/audio/sound.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_audio_sound.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_audio_sound.cpp$(DependSuffix) -MM src/audio/sound.cpp

$(IntermediateDirectory)/src_audio_sound.cpp$(PreprocessSuffix): src/audio/sound.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_audio_sound.cpp$(PreprocessSuffix) src/audio/sound.cpp

$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix): src/main.cpp $(IntermediateDirectory)/src_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main.cpp$(DependSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main.cpp$(DependSuffix) -MM src/main.cpp

$(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix): src/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix) src/main.cpp

$(IntermediateDirectory)/src_framework_navmesh.cpp$(ObjectSuffix): src/framework/navmesh.cpp $(IntermediateDirectory)/src_framework_navmesh.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/navmesh.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_navmesh.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_navmesh.cpp$(DependSuffix): src/framework/navmesh.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_navmesh.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_navmesh.cpp$(DependSuffix) -MM src/framework/navmesh.cpp

$(IntermediateDirectory)/src_framework_navmesh.cpp$(PreprocessSuffix): src/framework/navmesh.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_navmesh.cpp$(PreprocessSuffix) src/framework/navmesh.cpp

$(IntermediateDirectory)/src_framework_settings.cpp$(ObjectSuffix): src/framework/settings.cpp $(IntermediateDirectory)/src_framework_settings.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/settings.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_settings.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_settings.cpp$(DependSuffix): src/framework/settings.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_settings.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_settings.cpp$(DependSuffix) -MM src/framework/settings.cpp

$(IntermediateDirectory)/src_framework_settings.cpp$(PreprocessSuffix): src/framework/settings.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_settings.cpp$(PreprocessSuffix) src/framework/settings.cpp

$(IntermediateDirectory)/src_components_controller.cpp$(ObjectSuffix): src/components/controller.cpp $(IntermediateDirectory)/src_components_controller.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/controller.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_controller.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_controller.cpp$(DependSuffix): src/components/controller.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_controller.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_controller.cpp$(DependSuffix) -MM src/components/controller.cpp

$(IntermediateDirectory)/src_components_controller.cpp$(PreprocessSuffix): src/components/controller.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_controller.cpp$(PreprocessSuffix) src/components/controller.cpp

$(IntermediateDirectory)/src_audio_spatial_pathtracer.cpp$(ObjectSuffix): src/audio/spatial/pathtracer.cpp $(IntermediateDirectory)/src_audio_spatial_pathtracer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/audio/spatial/pathtracer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_audio_spatial_pathtracer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_audio_spatial_pathtracer.cpp$(DependSuffix): src/audio/spatial/pathtracer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_audio_spatial_pathtracer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_audio_spatial_pathtracer.cpp$(DependSuffix) -MM src/audio/spatial/pathtracer.cpp

$(IntermediateDirectory)/src_audio_spatial_pathtracer.cpp$(PreprocessSuffix): src/audio/spatial/pathtracer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_audio_spatial_pathtracer.cpp$(PreprocessSuffix) src/audio/spatial/pathtracer.cpp

$(IntermediateDirectory)/src_components_particle.cpp$(ObjectSuffix): src/components/particle.cpp $(IntermediateDirectory)/src_components_particle.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/particle.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_particle.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_particle.cpp$(DependSuffix): src/components/particle.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_particle.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_particle.cpp$(DependSuffix) -MM src/components/particle.cpp

$(IntermediateDirectory)/src_components_particle.cpp$(PreprocessSuffix): src/components/particle.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_particle.cpp$(PreprocessSuffix) src/components/particle.cpp

$(IntermediateDirectory)/src_framework_message.cpp$(ObjectSuffix): src/framework/message.cpp $(IntermediateDirectory)/src_framework_message.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/message.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_message.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_message.cpp$(DependSuffix): src/framework/message.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_message.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_message.cpp$(DependSuffix) -MM src/framework/message.cpp

$(IntermediateDirectory)/src_framework_message.cpp$(PreprocessSuffix): src/framework/message.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_message.cpp$(PreprocessSuffix) src/framework/message.cpp

$(IntermediateDirectory)/src_entities_marker.cpp$(ObjectSuffix): src/entities/marker.cpp $(IntermediateDirectory)/src_entities_marker.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/marker.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_marker.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_marker.cpp$(DependSuffix): src/entities/marker.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_marker.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_marker.cpp$(DependSuffix) -MM src/entities/marker.cpp

$(IntermediateDirectory)/src_entities_marker.cpp$(PreprocessSuffix): src/entities/marker.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_marker.cpp$(PreprocessSuffix) src/entities/marker.cpp

$(IntermediateDirectory)/src_framework_gui.cpp$(ObjectSuffix): src/framework/gui.cpp $(IntermediateDirectory)/src_framework_gui.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/gui.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_gui.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_gui.cpp$(DependSuffix): src/framework/gui.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_gui.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_gui.cpp$(DependSuffix) -MM src/framework/gui.cpp

$(IntermediateDirectory)/src_framework_gui.cpp$(PreprocessSuffix): src/framework/gui.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_gui.cpp$(PreprocessSuffix) src/framework/gui.cpp

$(IntermediateDirectory)/src_framework_core.cpp$(ObjectSuffix): src/framework/core.cpp $(IntermediateDirectory)/src_framework_core.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/core.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_core.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_core.cpp$(DependSuffix): src/framework/core.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_core.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_core.cpp$(DependSuffix) -MM src/framework/core.cpp

$(IntermediateDirectory)/src_framework_core.cpp$(PreprocessSuffix): src/framework/core.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_core.cpp$(PreprocessSuffix) src/framework/core.cpp

$(IntermediateDirectory)/src_platform_time.cpp$(ObjectSuffix): src/platform/time.cpp $(IntermediateDirectory)/src_platform_time.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/platform/time.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_platform_time.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_platform_time.cpp$(DependSuffix): src/platform/time.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_platform_time.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_platform_time.cpp$(DependSuffix) -MM src/platform/time.cpp

$(IntermediateDirectory)/src_platform_time.cpp$(PreprocessSuffix): src/platform/time.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_platform_time.cpp$(PreprocessSuffix) src/platform/time.cpp

$(IntermediateDirectory)/src_entities_sound.cpp$(ObjectSuffix): src/entities/sound.cpp $(IntermediateDirectory)/src_entities_sound.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/sound.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_sound.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_sound.cpp$(DependSuffix): src/entities/sound.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_sound.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_sound.cpp$(DependSuffix) -MM src/entities/sound.cpp

$(IntermediateDirectory)/src_entities_sound.cpp$(PreprocessSuffix): src/entities/sound.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_sound.cpp$(PreprocessSuffix) src/entities/sound.cpp

$(IntermediateDirectory)/src_framework_navigation.cpp$(ObjectSuffix): src/framework/navigation.cpp $(IntermediateDirectory)/src_framework_navigation.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/navigation.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_navigation.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_navigation.cpp$(DependSuffix): src/framework/navigation.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_navigation.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_navigation.cpp$(DependSuffix) -MM src/framework/navigation.cpp

$(IntermediateDirectory)/src_framework_navigation.cpp$(PreprocessSuffix): src/framework/navigation.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_navigation.cpp$(PreprocessSuffix) src/framework/navigation.cpp

$(IntermediateDirectory)/src_framework_async.cpp$(ObjectSuffix): src/framework/async.cpp $(IntermediateDirectory)/src_framework_async.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/async.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_async.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_async.cpp$(DependSuffix): src/framework/async.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_async.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_async.cpp$(DependSuffix) -MM src/framework/async.cpp

$(IntermediateDirectory)/src_framework_async.cpp$(PreprocessSuffix): src/framework/async.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_async.cpp$(PreprocessSuffix) src/framework/async.cpp

$(IntermediateDirectory)/src_framework_worldcell.cpp$(ObjectSuffix): src/framework/worldcell.cpp $(IntermediateDirectory)/src_framework_worldcell.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/worldcell.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_worldcell.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_worldcell.cpp$(DependSuffix): src/framework/worldcell.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_worldcell.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_worldcell.cpp$(DependSuffix) -MM src/framework/worldcell.cpp

$(IntermediateDirectory)/src_framework_worldcell.cpp$(PreprocessSuffix): src/framework/worldcell.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_worldcell.cpp$(PreprocessSuffix) src/framework/worldcell.cpp

$(IntermediateDirectory)/src_framework_uid.cpp$(ObjectSuffix): src/framework/uid.cpp $(IntermediateDirectory)/src_framework_uid.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/uid.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_uid.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_uid.cpp$(DependSuffix): src/framework/uid.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_uid.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_uid.cpp$(DependSuffix) -MM src/framework/uid.cpp

$(IntermediateDirectory)/src_framework_uid.cpp$(PreprocessSuffix): src/framework/uid.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_uid.cpp$(PreprocessSuffix) src/framework/uid.cpp

$(IntermediateDirectory)/src_framework_stats.cpp$(ObjectSuffix): src/framework/stats.cpp $(IntermediateDirectory)/src_framework_stats.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/stats.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_stats.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_stats.cpp$(DependSuffix): src/framework/stats.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_stats.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_stats.cpp$(DependSuffix) -MM src/framework/stats.cpp

$(IntermediateDirectory)/src_framework_stats.cpp$(PreprocessSuffix): src/framework/stats.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_stats.cpp$(PreprocessSuffix) src/framework/stats.cpp

$(IntermediateDirectory)/src_framework_event.cpp$(ObjectSuffix): src/framework/event.cpp $(IntermediateDirectory)/src_framework_event.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/event.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_event.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_event.cpp$(DependSuffix): src/framework/event.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_event.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_event.cpp$(DependSuffix) -MM src/framework/event.cpp

$(IntermediateDirectory)/src_framework_event.cpp$(PreprocessSuffix): src/framework/event.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_event.cpp$(PreprocessSuffix) src/framework/event.cpp

$(IntermediateDirectory)/src_render_material.cpp$(ObjectSuffix): src/render/material.cpp $(IntermediateDirectory)/src_render_material.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/material.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_material.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_material.cpp$(DependSuffix): src/render/material.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_material.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_material.cpp$(DependSuffix) -MM src/render/material.cpp

$(IntermediateDirectory)/src_render_material.cpp$(PreprocessSuffix): src/render/material.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_material.cpp$(PreprocessSuffix) src/render/material.cpp

$(IntermediateDirectory)/src_framework_path.cpp$(ObjectSuffix): src/framework/path.cpp $(IntermediateDirectory)/src_framework_path.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/path.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_path.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_path.cpp$(DependSuffix): src/framework/path.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_path.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_path.cpp$(DependSuffix) -MM src/framework/path.cpp

$(IntermediateDirectory)/src_framework_path.cpp$(PreprocessSuffix): src/framework/path.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_path.cpp$(PreprocessSuffix) src/framework/path.cpp

$(IntermediateDirectory)/src_framework_math.cpp$(ObjectSuffix): src/framework/math.cpp $(IntermediateDirectory)/src_framework_math.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/math.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_math.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_math.cpp$(DependSuffix): src/framework/math.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_math.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_math.cpp$(DependSuffix) -MM src/framework/math.cpp

$(IntermediateDirectory)/src_framework_math.cpp$(PreprocessSuffix): src/framework/math.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_math.cpp$(PreprocessSuffix) src/framework/math.cpp

$(IntermediateDirectory)/src_platform_image.cpp$(ObjectSuffix): src/platform/image.cpp $(IntermediateDirectory)/src_platform_image.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/platform/image.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_platform_image.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_platform_image.cpp$(DependSuffix): src/platform/image.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_platform_image.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_platform_image.cpp$(DependSuffix) -MM src/platform/image.cpp

$(IntermediateDirectory)/src_platform_image.cpp$(PreprocessSuffix): src/platform/image.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_platform_image.cpp$(PreprocessSuffix) src/platform/image.cpp

$(IntermediateDirectory)/src_platform_file.cpp$(ObjectSuffix): src/platform/file.cpp $(IntermediateDirectory)/src_platform_file.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/platform/file.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_platform_file.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_platform_file.cpp$(DependSuffix): src/platform/file.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_platform_file.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_platform_file.cpp$(DependSuffix) -MM src/platform/file.cpp

$(IntermediateDirectory)/src_platform_file.cpp$(PreprocessSuffix): src/platform/file.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_platform_file.cpp$(PreprocessSuffix) src/platform/file.cpp

$(IntermediateDirectory)/src_physics_physics.cpp$(ObjectSuffix): src/physics/physics.cpp $(IntermediateDirectory)/src_physics_physics.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/physics/physics.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_physics_physics.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_physics_physics.cpp$(DependSuffix): src/physics/physics.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_physics_physics.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_physics_physics.cpp$(DependSuffix) -MM src/physics/physics.cpp

$(IntermediateDirectory)/src_physics_physics.cpp$(PreprocessSuffix): src/physics/physics.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_physics_physics.cpp$(PreprocessSuffix) src/physics/physics.cpp

$(IntermediateDirectory)/src_render_animation.cpp$(ObjectSuffix): src/render/animation.cpp $(IntermediateDirectory)/src_render_animation.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/animation.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_animation.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_animation.cpp$(DependSuffix): src/render/animation.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_animation.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_animation.cpp$(DependSuffix) -MM src/render/animation.cpp

$(IntermediateDirectory)/src_render_animation.cpp$(PreprocessSuffix): src/render/animation.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_animation.cpp$(PreprocessSuffix) src/render/animation.cpp

$(IntermediateDirectory)/src_physics_collisionmodel.cpp$(ObjectSuffix): src/physics/collisionmodel.cpp $(IntermediateDirectory)/src_physics_collisionmodel.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/physics/collisionmodel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_physics_collisionmodel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_physics_collisionmodel.cpp$(DependSuffix): src/physics/collisionmodel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_physics_collisionmodel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_physics_collisionmodel.cpp$(DependSuffix) -MM src/physics/collisionmodel.cpp

$(IntermediateDirectory)/src_physics_collisionmodel.cpp$(PreprocessSuffix): src/physics/collisionmodel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_physics_collisionmodel.cpp$(PreprocessSuffix) src/physics/collisionmodel.cpp

$(IntermediateDirectory)/src_framework_cli.cpp$(ObjectSuffix): src/framework/cli.cpp $(IntermediateDirectory)/src_framework_cli.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/cli.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_cli.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_cli.cpp$(DependSuffix): src/framework/cli.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_cli.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_cli.cpp$(DependSuffix) -MM src/framework/cli.cpp

$(IntermediateDirectory)/src_framework_cli.cpp$(PreprocessSuffix): src/framework/cli.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_cli.cpp$(PreprocessSuffix) src/framework/cli.cpp

$(IntermediateDirectory)/src_components_trigger.cpp$(ObjectSuffix): src/components/trigger.cpp $(IntermediateDirectory)/src_components_trigger.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/trigger.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_trigger.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_trigger.cpp$(DependSuffix): src/components/trigger.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_trigger.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_trigger.cpp$(DependSuffix) -MM src/components/trigger.cpp

$(IntermediateDirectory)/src_components_trigger.cpp$(PreprocessSuffix): src/components/trigger.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_trigger.cpp$(PreprocessSuffix) src/components/trigger.cpp

$(IntermediateDirectory)/src_render_aabb.cpp$(ObjectSuffix): src/render/aabb.cpp $(IntermediateDirectory)/src_render_aabb.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/aabb.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_aabb.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_aabb.cpp$(DependSuffix): src/render/aabb.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_aabb.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_aabb.cpp$(DependSuffix) -MM src/render/aabb.cpp

$(IntermediateDirectory)/src_render_aabb.cpp$(PreprocessSuffix): src/render/aabb.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_aabb.cpp$(PreprocessSuffix) src/render/aabb.cpp

$(IntermediateDirectory)/src_render_error.cpp$(ObjectSuffix): src/render/error.cpp $(IntermediateDirectory)/src_render_error.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/error.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_error.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_error.cpp$(DependSuffix): src/render/error.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_error.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_error.cpp$(DependSuffix) -MM src/render/error.cpp

$(IntermediateDirectory)/src_render_error.cpp$(PreprocessSuffix): src/render/error.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_error.cpp$(PreprocessSuffix) src/render/error.cpp

$(IntermediateDirectory)/src_framework_signal.cpp$(ObjectSuffix): src/framework/signal.cpp $(IntermediateDirectory)/src_framework_signal.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/signal.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_signal.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_signal.cpp$(DependSuffix): src/framework/signal.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_signal.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_signal.cpp$(DependSuffix) -MM src/framework/signal.cpp

$(IntermediateDirectory)/src_framework_signal.cpp$(PreprocessSuffix): src/framework/signal.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_signal.cpp$(PreprocessSuffix) src/framework/signal.cpp

$(IntermediateDirectory)/src_render_model.cpp$(ObjectSuffix): src/render/model.cpp $(IntermediateDirectory)/src_render_model.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/model.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_model.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_model.cpp$(DependSuffix): src/render/model.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_model.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_model.cpp$(DependSuffix) -MM src/render/model.cpp

$(IntermediateDirectory)/src_render_model.cpp$(PreprocessSuffix): src/render/model.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_model.cpp$(PreprocessSuffix) src/render/model.cpp

$(IntermediateDirectory)/src_framework_logging.cpp$(ObjectSuffix): src/framework/logging.cpp $(IntermediateDirectory)/src_framework_logging.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/logging.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_logging.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_logging.cpp$(DependSuffix): src/framework/logging.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_logging.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_logging.cpp$(DependSuffix) -MM src/framework/logging.cpp

$(IntermediateDirectory)/src_framework_logging.cpp$(PreprocessSuffix): src/framework/logging.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_logging.cpp$(PreprocessSuffix) src/framework/logging.cpp

$(IntermediateDirectory)/src_render_sprite.cpp$(ObjectSuffix): src/render/sprite.cpp $(IntermediateDirectory)/src_render_sprite.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/sprite.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_sprite.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_sprite.cpp$(DependSuffix): src/render/sprite.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_sprite.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_sprite.cpp$(DependSuffix) -MM src/render/sprite.cpp

$(IntermediateDirectory)/src_render_sprite.cpp$(PreprocessSuffix): src/render/sprite.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_sprite.cpp$(PreprocessSuffix) src/render/sprite.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(IntermediateDirectory)


