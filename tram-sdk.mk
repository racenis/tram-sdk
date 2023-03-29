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
Date                   :=29/03/2023
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
LinkOptions            :=  -lglfw3 -lgdi32 -lopengl32
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)./src $(IncludeSwitch)./libraries $(IncludeSwitch)./libraries/headers 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)BulletSoftBody $(LibrarySwitch)BulletDynamics $(LibrarySwitch)BulletCollision $(LibrarySwitch)LinearMath $(LibrarySwitch)glfw3 $(LibrarySwitch)OpenAL32 
ArLibs                 :=  "libBulletSoftBody.a" "libBulletDynamics.a" "libBulletCollision.a" "libLinearMath.a" "libglfw3.a" "libOpenAL32.a" 
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
Objects0=$(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_audio_openal_openal.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_menu_menu.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_player.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_tram.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_lamp.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_marker.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_material.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_armature.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_renderer.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_physics_collisionmodel.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_platform_file.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_opengl_renderer.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_controllercomponent.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_opengl_shader.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_sprite.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_armaturecomponent.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_bus.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_model.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_particlecomponent.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_framework_math.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_core.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_extensions_camera_camera.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_physicscomponent.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_rendercomponent.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_triggercomponent.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_audio_audio.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_ui.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_render_misc.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_physics_physics.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_framework_language.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_entitycomponents.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_message.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_navigation.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_spritecomponent.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_components_audiocomponent.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_staticworldobject.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_gui.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_render.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_framework_net.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_crate.cpp$(ObjectSuffix) 

Objects1=$(IntermediateDirectory)/src_framework_entity.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_mongus.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_logging.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_stats.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_async.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_worldcell.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_uid.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_audio_sound.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_framework_system.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_framework_event.cpp$(ObjectSuffix) 



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
$(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(ObjectSuffix): src/physics/bullet/bullet.cpp $(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/physics/bullet/bullet.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(DependSuffix): src/physics/bullet/bullet.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(DependSuffix) -MM src/physics/bullet/bullet.cpp

$(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(PreprocessSuffix): src/physics/bullet/bullet.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_physics_bullet_bullet.cpp$(PreprocessSuffix) src/physics/bullet/bullet.cpp

$(IntermediateDirectory)/src_audio_openal_openal.cpp$(ObjectSuffix): src/audio/openal/openal.cpp $(IntermediateDirectory)/src_audio_openal_openal.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/audio/openal/openal.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_audio_openal_openal.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_audio_openal_openal.cpp$(DependSuffix): src/audio/openal/openal.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_audio_openal_openal.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_audio_openal_openal.cpp$(DependSuffix) -MM src/audio/openal/openal.cpp

$(IntermediateDirectory)/src_audio_openal_openal.cpp$(PreprocessSuffix): src/audio/openal/openal.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_audio_openal_openal.cpp$(PreprocessSuffix) src/audio/openal/openal.cpp

$(IntermediateDirectory)/src_extensions_menu_menu.cpp$(ObjectSuffix): src/extensions/menu/menu.cpp $(IntermediateDirectory)/src_extensions_menu_menu.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/menu/menu.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_menu_menu.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_menu_menu.cpp$(DependSuffix): src/extensions/menu/menu.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_menu_menu.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_menu_menu.cpp$(DependSuffix) -MM src/extensions/menu/menu.cpp

$(IntermediateDirectory)/src_extensions_menu_menu.cpp$(PreprocessSuffix): src/extensions/menu/menu.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_menu_menu.cpp$(PreprocessSuffix) src/extensions/menu/menu.cpp

$(IntermediateDirectory)/src_entities_player.cpp$(ObjectSuffix): src/entities/player.cpp $(IntermediateDirectory)/src_entities_player.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/player.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_player.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_player.cpp$(DependSuffix): src/entities/player.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_player.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_player.cpp$(DependSuffix) -MM src/entities/player.cpp

$(IntermediateDirectory)/src_entities_player.cpp$(PreprocessSuffix): src/entities/player.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_player.cpp$(PreprocessSuffix) src/entities/player.cpp

$(IntermediateDirectory)/src_entities_tram.cpp$(ObjectSuffix): src/entities/tram.cpp $(IntermediateDirectory)/src_entities_tram.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/tram.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_tram.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_tram.cpp$(DependSuffix): src/entities/tram.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_tram.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_tram.cpp$(DependSuffix) -MM src/entities/tram.cpp

$(IntermediateDirectory)/src_entities_tram.cpp$(PreprocessSuffix): src/entities/tram.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_tram.cpp$(PreprocessSuffix) src/entities/tram.cpp

$(IntermediateDirectory)/src_entities_lamp.cpp$(ObjectSuffix): src/entities/lamp.cpp $(IntermediateDirectory)/src_entities_lamp.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/lamp.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_lamp.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_lamp.cpp$(DependSuffix): src/entities/lamp.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_lamp.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_lamp.cpp$(DependSuffix) -MM src/entities/lamp.cpp

$(IntermediateDirectory)/src_entities_lamp.cpp$(PreprocessSuffix): src/entities/lamp.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_lamp.cpp$(PreprocessSuffix) src/entities/lamp.cpp

$(IntermediateDirectory)/src_entities_marker.cpp$(ObjectSuffix): src/entities/marker.cpp $(IntermediateDirectory)/src_entities_marker.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/marker.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_marker.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_marker.cpp$(DependSuffix): src/entities/marker.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_marker.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_marker.cpp$(DependSuffix) -MM src/entities/marker.cpp

$(IntermediateDirectory)/src_entities_marker.cpp$(PreprocessSuffix): src/entities/marker.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_marker.cpp$(PreprocessSuffix) src/entities/marker.cpp

$(IntermediateDirectory)/src_render_material.cpp$(ObjectSuffix): src/render/material.cpp $(IntermediateDirectory)/src_render_material.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/material.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_material.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_material.cpp$(DependSuffix): src/render/material.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_material.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_material.cpp$(DependSuffix) -MM src/render/material.cpp

$(IntermediateDirectory)/src_render_material.cpp$(PreprocessSuffix): src/render/material.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_material.cpp$(PreprocessSuffix) src/render/material.cpp

$(IntermediateDirectory)/src_render_armature.cpp$(ObjectSuffix): src/render/armature.cpp $(IntermediateDirectory)/src_render_armature.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/armature.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_armature.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_armature.cpp$(DependSuffix): src/render/armature.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_armature.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_armature.cpp$(DependSuffix) -MM src/render/armature.cpp

$(IntermediateDirectory)/src_render_armature.cpp$(PreprocessSuffix): src/render/armature.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_armature.cpp$(PreprocessSuffix) src/render/armature.cpp

$(IntermediateDirectory)/src_render_renderer.cpp$(ObjectSuffix): src/render/renderer.cpp $(IntermediateDirectory)/src_render_renderer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/renderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_renderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_renderer.cpp$(DependSuffix): src/render/renderer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_renderer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_renderer.cpp$(DependSuffix) -MM src/render/renderer.cpp

$(IntermediateDirectory)/src_render_renderer.cpp$(PreprocessSuffix): src/render/renderer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_renderer.cpp$(PreprocessSuffix) src/render/renderer.cpp

$(IntermediateDirectory)/src_physics_collisionmodel.cpp$(ObjectSuffix): src/physics/collisionmodel.cpp $(IntermediateDirectory)/src_physics_collisionmodel.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/physics/collisionmodel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_physics_collisionmodel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_physics_collisionmodel.cpp$(DependSuffix): src/physics/collisionmodel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_physics_collisionmodel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_physics_collisionmodel.cpp$(DependSuffix) -MM src/physics/collisionmodel.cpp

$(IntermediateDirectory)/src_physics_collisionmodel.cpp$(PreprocessSuffix): src/physics/collisionmodel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_physics_collisionmodel.cpp$(PreprocessSuffix) src/physics/collisionmodel.cpp

$(IntermediateDirectory)/src_platform_file.cpp$(ObjectSuffix): src/platform/file.cpp $(IntermediateDirectory)/src_platform_file.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/platform/file.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_platform_file.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_platform_file.cpp$(DependSuffix): src/platform/file.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_platform_file.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_platform_file.cpp$(DependSuffix) -MM src/platform/file.cpp

$(IntermediateDirectory)/src_platform_file.cpp$(PreprocessSuffix): src/platform/file.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_platform_file.cpp$(PreprocessSuffix) src/platform/file.cpp

$(IntermediateDirectory)/src_render_opengl_renderer.cpp$(ObjectSuffix): src/render/opengl/renderer.cpp $(IntermediateDirectory)/src_render_opengl_renderer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/opengl/renderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_opengl_renderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_opengl_renderer.cpp$(DependSuffix): src/render/opengl/renderer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_opengl_renderer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_opengl_renderer.cpp$(DependSuffix) -MM src/render/opengl/renderer.cpp

$(IntermediateDirectory)/src_render_opengl_renderer.cpp$(PreprocessSuffix): src/render/opengl/renderer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_opengl_renderer.cpp$(PreprocessSuffix) src/render/opengl/renderer.cpp

$(IntermediateDirectory)/src_components_controllercomponent.cpp$(ObjectSuffix): src/components/controllercomponent.cpp $(IntermediateDirectory)/src_components_controllercomponent.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/controllercomponent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_controllercomponent.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_controllercomponent.cpp$(DependSuffix): src/components/controllercomponent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_controllercomponent.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_controllercomponent.cpp$(DependSuffix) -MM src/components/controllercomponent.cpp

$(IntermediateDirectory)/src_components_controllercomponent.cpp$(PreprocessSuffix): src/components/controllercomponent.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_controllercomponent.cpp$(PreprocessSuffix) src/components/controllercomponent.cpp

$(IntermediateDirectory)/src_render_opengl_shader.cpp$(ObjectSuffix): src/render/opengl/shader.cpp $(IntermediateDirectory)/src_render_opengl_shader.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/opengl/shader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_opengl_shader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_opengl_shader.cpp$(DependSuffix): src/render/opengl/shader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_opengl_shader.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_opengl_shader.cpp$(DependSuffix) -MM src/render/opengl/shader.cpp

$(IntermediateDirectory)/src_render_opengl_shader.cpp$(PreprocessSuffix): src/render/opengl/shader.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_opengl_shader.cpp$(PreprocessSuffix) src/render/opengl/shader.cpp

$(IntermediateDirectory)/src_render_sprite.cpp$(ObjectSuffix): src/render/sprite.cpp $(IntermediateDirectory)/src_render_sprite.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/sprite.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_sprite.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_sprite.cpp$(DependSuffix): src/render/sprite.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_sprite.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_sprite.cpp$(DependSuffix) -MM src/render/sprite.cpp

$(IntermediateDirectory)/src_render_sprite.cpp$(PreprocessSuffix): src/render/sprite.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_sprite.cpp$(PreprocessSuffix) src/render/sprite.cpp

$(IntermediateDirectory)/src_components_armaturecomponent.cpp$(ObjectSuffix): src/components/armaturecomponent.cpp $(IntermediateDirectory)/src_components_armaturecomponent.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/armaturecomponent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_armaturecomponent.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_armaturecomponent.cpp$(DependSuffix): src/components/armaturecomponent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_armaturecomponent.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_armaturecomponent.cpp$(DependSuffix) -MM src/components/armaturecomponent.cpp

$(IntermediateDirectory)/src_components_armaturecomponent.cpp$(PreprocessSuffix): src/components/armaturecomponent.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_armaturecomponent.cpp$(PreprocessSuffix) src/components/armaturecomponent.cpp

$(IntermediateDirectory)/src_entities_bus.cpp$(ObjectSuffix): src/entities/bus.cpp $(IntermediateDirectory)/src_entities_bus.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/bus.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_bus.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_bus.cpp$(DependSuffix): src/entities/bus.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_bus.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_bus.cpp$(DependSuffix) -MM src/entities/bus.cpp

$(IntermediateDirectory)/src_entities_bus.cpp$(PreprocessSuffix): src/entities/bus.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_bus.cpp$(PreprocessSuffix) src/entities/bus.cpp

$(IntermediateDirectory)/src_render_model.cpp$(ObjectSuffix): src/render/model.cpp $(IntermediateDirectory)/src_render_model.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/model.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_model.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_model.cpp$(DependSuffix): src/render/model.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_model.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_model.cpp$(DependSuffix) -MM src/render/model.cpp

$(IntermediateDirectory)/src_render_model.cpp$(PreprocessSuffix): src/render/model.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_model.cpp$(PreprocessSuffix) src/render/model.cpp

$(IntermediateDirectory)/src_components_particlecomponent.cpp$(ObjectSuffix): src/components/particlecomponent.cpp $(IntermediateDirectory)/src_components_particlecomponent.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/particlecomponent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_particlecomponent.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_particlecomponent.cpp$(DependSuffix): src/components/particlecomponent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_particlecomponent.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_particlecomponent.cpp$(DependSuffix) -MM src/components/particlecomponent.cpp

$(IntermediateDirectory)/src_components_particlecomponent.cpp$(PreprocessSuffix): src/components/particlecomponent.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_particlecomponent.cpp$(PreprocessSuffix) src/components/particlecomponent.cpp

$(IntermediateDirectory)/src_framework_math.cpp$(ObjectSuffix): src/framework/math.cpp $(IntermediateDirectory)/src_framework_math.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/math.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_math.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_math.cpp$(DependSuffix): src/framework/math.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_math.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_math.cpp$(DependSuffix) -MM src/framework/math.cpp

$(IntermediateDirectory)/src_framework_math.cpp$(PreprocessSuffix): src/framework/math.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_math.cpp$(PreprocessSuffix) src/framework/math.cpp

$(IntermediateDirectory)/src_framework_core.cpp$(ObjectSuffix): src/framework/core.cpp $(IntermediateDirectory)/src_framework_core.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/core.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_core.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_core.cpp$(DependSuffix): src/framework/core.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_core.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_core.cpp$(DependSuffix) -MM src/framework/core.cpp

$(IntermediateDirectory)/src_framework_core.cpp$(PreprocessSuffix): src/framework/core.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_core.cpp$(PreprocessSuffix) src/framework/core.cpp

$(IntermediateDirectory)/src_extensions_camera_camera.cpp$(ObjectSuffix): src/extensions/camera/camera.cpp $(IntermediateDirectory)/src_extensions_camera_camera.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/extensions/camera/camera.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_extensions_camera_camera.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_extensions_camera_camera.cpp$(DependSuffix): src/extensions/camera/camera.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_extensions_camera_camera.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_extensions_camera_camera.cpp$(DependSuffix) -MM src/extensions/camera/camera.cpp

$(IntermediateDirectory)/src_extensions_camera_camera.cpp$(PreprocessSuffix): src/extensions/camera/camera.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_extensions_camera_camera.cpp$(PreprocessSuffix) src/extensions/camera/camera.cpp

$(IntermediateDirectory)/src_components_physicscomponent.cpp$(ObjectSuffix): src/components/physicscomponent.cpp $(IntermediateDirectory)/src_components_physicscomponent.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/physicscomponent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_physicscomponent.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_physicscomponent.cpp$(DependSuffix): src/components/physicscomponent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_physicscomponent.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_physicscomponent.cpp$(DependSuffix) -MM src/components/physicscomponent.cpp

$(IntermediateDirectory)/src_components_physicscomponent.cpp$(PreprocessSuffix): src/components/physicscomponent.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_physicscomponent.cpp$(PreprocessSuffix) src/components/physicscomponent.cpp

$(IntermediateDirectory)/src_components_rendercomponent.cpp$(ObjectSuffix): src/components/rendercomponent.cpp $(IntermediateDirectory)/src_components_rendercomponent.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/rendercomponent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_rendercomponent.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_rendercomponent.cpp$(DependSuffix): src/components/rendercomponent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_rendercomponent.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_rendercomponent.cpp$(DependSuffix) -MM src/components/rendercomponent.cpp

$(IntermediateDirectory)/src_components_rendercomponent.cpp$(PreprocessSuffix): src/components/rendercomponent.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_rendercomponent.cpp$(PreprocessSuffix) src/components/rendercomponent.cpp

$(IntermediateDirectory)/src_components_triggercomponent.cpp$(ObjectSuffix): src/components/triggercomponent.cpp $(IntermediateDirectory)/src_components_triggercomponent.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/triggercomponent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_triggercomponent.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_triggercomponent.cpp$(DependSuffix): src/components/triggercomponent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_triggercomponent.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_triggercomponent.cpp$(DependSuffix) -MM src/components/triggercomponent.cpp

$(IntermediateDirectory)/src_components_triggercomponent.cpp$(PreprocessSuffix): src/components/triggercomponent.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_triggercomponent.cpp$(PreprocessSuffix) src/components/triggercomponent.cpp

$(IntermediateDirectory)/src_audio_audio.cpp$(ObjectSuffix): src/audio/audio.cpp $(IntermediateDirectory)/src_audio_audio.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/audio/audio.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_audio_audio.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_audio_audio.cpp$(DependSuffix): src/audio/audio.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_audio_audio.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_audio_audio.cpp$(DependSuffix) -MM src/audio/audio.cpp

$(IntermediateDirectory)/src_audio_audio.cpp$(PreprocessSuffix): src/audio/audio.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_audio_audio.cpp$(PreprocessSuffix) src/audio/audio.cpp

$(IntermediateDirectory)/src_framework_ui.cpp$(ObjectSuffix): src/framework/ui.cpp $(IntermediateDirectory)/src_framework_ui.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/ui.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_ui.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_ui.cpp$(DependSuffix): src/framework/ui.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_ui.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_ui.cpp$(DependSuffix) -MM src/framework/ui.cpp

$(IntermediateDirectory)/src_framework_ui.cpp$(PreprocessSuffix): src/framework/ui.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_ui.cpp$(PreprocessSuffix) src/framework/ui.cpp

$(IntermediateDirectory)/src_render_render_misc.cpp$(ObjectSuffix): src/render/render_misc.cpp $(IntermediateDirectory)/src_render_render_misc.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/render_misc.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_render_misc.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_render_misc.cpp$(DependSuffix): src/render/render_misc.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_render_misc.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_render_misc.cpp$(DependSuffix) -MM src/render/render_misc.cpp

$(IntermediateDirectory)/src_render_render_misc.cpp$(PreprocessSuffix): src/render/render_misc.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_render_misc.cpp$(PreprocessSuffix) src/render/render_misc.cpp

$(IntermediateDirectory)/src_physics_physics.cpp$(ObjectSuffix): src/physics/physics.cpp $(IntermediateDirectory)/src_physics_physics.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/physics/physics.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_physics_physics.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_physics_physics.cpp$(DependSuffix): src/physics/physics.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_physics_physics.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_physics_physics.cpp$(DependSuffix) -MM src/physics/physics.cpp

$(IntermediateDirectory)/src_physics_physics.cpp$(PreprocessSuffix): src/physics/physics.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_physics_physics.cpp$(PreprocessSuffix) src/physics/physics.cpp

$(IntermediateDirectory)/src_framework_language.cpp$(ObjectSuffix): src/framework/language.cpp $(IntermediateDirectory)/src_framework_language.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/language.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_language.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_language.cpp$(DependSuffix): src/framework/language.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_language.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_language.cpp$(DependSuffix) -MM src/framework/language.cpp

$(IntermediateDirectory)/src_framework_language.cpp$(PreprocessSuffix): src/framework/language.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_language.cpp$(PreprocessSuffix) src/framework/language.cpp

$(IntermediateDirectory)/src_components_entitycomponents.cpp$(ObjectSuffix): src/components/entitycomponents.cpp $(IntermediateDirectory)/src_components_entitycomponents.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/entitycomponents.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_entitycomponents.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_entitycomponents.cpp$(DependSuffix): src/components/entitycomponents.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_entitycomponents.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_entitycomponents.cpp$(DependSuffix) -MM src/components/entitycomponents.cpp

$(IntermediateDirectory)/src_components_entitycomponents.cpp$(PreprocessSuffix): src/components/entitycomponents.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_entitycomponents.cpp$(PreprocessSuffix) src/components/entitycomponents.cpp

$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix): src/main.cpp $(IntermediateDirectory)/src_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main.cpp$(DependSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main.cpp$(DependSuffix) -MM src/main.cpp

$(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix): src/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix) src/main.cpp

$(IntermediateDirectory)/src_framework_message.cpp$(ObjectSuffix): src/framework/message.cpp $(IntermediateDirectory)/src_framework_message.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/message.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_message.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_message.cpp$(DependSuffix): src/framework/message.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_message.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_message.cpp$(DependSuffix) -MM src/framework/message.cpp

$(IntermediateDirectory)/src_framework_message.cpp$(PreprocessSuffix): src/framework/message.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_message.cpp$(PreprocessSuffix) src/framework/message.cpp

$(IntermediateDirectory)/src_framework_navigation.cpp$(ObjectSuffix): src/framework/navigation.cpp $(IntermediateDirectory)/src_framework_navigation.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/navigation.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_navigation.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_navigation.cpp$(DependSuffix): src/framework/navigation.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_navigation.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_navigation.cpp$(DependSuffix) -MM src/framework/navigation.cpp

$(IntermediateDirectory)/src_framework_navigation.cpp$(PreprocessSuffix): src/framework/navigation.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_navigation.cpp$(PreprocessSuffix) src/framework/navigation.cpp

$(IntermediateDirectory)/src_components_spritecomponent.cpp$(ObjectSuffix): src/components/spritecomponent.cpp $(IntermediateDirectory)/src_components_spritecomponent.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/spritecomponent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_spritecomponent.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_spritecomponent.cpp$(DependSuffix): src/components/spritecomponent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_spritecomponent.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_spritecomponent.cpp$(DependSuffix) -MM src/components/spritecomponent.cpp

$(IntermediateDirectory)/src_components_spritecomponent.cpp$(PreprocessSuffix): src/components/spritecomponent.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_spritecomponent.cpp$(PreprocessSuffix) src/components/spritecomponent.cpp

$(IntermediateDirectory)/src_components_audiocomponent.cpp$(ObjectSuffix): src/components/audiocomponent.cpp $(IntermediateDirectory)/src_components_audiocomponent.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/components/audiocomponent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_components_audiocomponent.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_components_audiocomponent.cpp$(DependSuffix): src/components/audiocomponent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_components_audiocomponent.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_components_audiocomponent.cpp$(DependSuffix) -MM src/components/audiocomponent.cpp

$(IntermediateDirectory)/src_components_audiocomponent.cpp$(PreprocessSuffix): src/components/audiocomponent.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_components_audiocomponent.cpp$(PreprocessSuffix) src/components/audiocomponent.cpp

$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(ObjectSuffix): src/entities/staticworldobject.cpp $(IntermediateDirectory)/src_entities_staticworldobject.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/staticworldobject.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(DependSuffix): src/entities/staticworldobject.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(DependSuffix) -MM src/entities/staticworldobject.cpp

$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(PreprocessSuffix): src/entities/staticworldobject.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_staticworldobject.cpp$(PreprocessSuffix) src/entities/staticworldobject.cpp

$(IntermediateDirectory)/src_framework_gui.cpp$(ObjectSuffix): src/framework/gui.cpp $(IntermediateDirectory)/src_framework_gui.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/gui.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_gui.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_gui.cpp$(DependSuffix): src/framework/gui.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_gui.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_gui.cpp$(DependSuffix) -MM src/framework/gui.cpp

$(IntermediateDirectory)/src_framework_gui.cpp$(PreprocessSuffix): src/framework/gui.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_gui.cpp$(PreprocessSuffix) src/framework/gui.cpp

$(IntermediateDirectory)/src_render_render.cpp$(ObjectSuffix): src/render/render.cpp $(IntermediateDirectory)/src_render_render.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/render.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_render.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_render.cpp$(DependSuffix): src/render/render.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_render.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_render.cpp$(DependSuffix) -MM src/render/render.cpp

$(IntermediateDirectory)/src_render_render.cpp$(PreprocessSuffix): src/render/render.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_render.cpp$(PreprocessSuffix) src/render/render.cpp

$(IntermediateDirectory)/src_framework_net.cpp$(ObjectSuffix): src/framework/net.cpp $(IntermediateDirectory)/src_framework_net.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/net.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_net.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_net.cpp$(DependSuffix): src/framework/net.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_net.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_net.cpp$(DependSuffix) -MM src/framework/net.cpp

$(IntermediateDirectory)/src_framework_net.cpp$(PreprocessSuffix): src/framework/net.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_net.cpp$(PreprocessSuffix) src/framework/net.cpp

$(IntermediateDirectory)/src_entities_crate.cpp$(ObjectSuffix): src/entities/crate.cpp $(IntermediateDirectory)/src_entities_crate.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/crate.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_crate.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_crate.cpp$(DependSuffix): src/entities/crate.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_crate.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_crate.cpp$(DependSuffix) -MM src/entities/crate.cpp

$(IntermediateDirectory)/src_entities_crate.cpp$(PreprocessSuffix): src/entities/crate.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_crate.cpp$(PreprocessSuffix) src/entities/crate.cpp

$(IntermediateDirectory)/src_framework_entity.cpp$(ObjectSuffix): src/framework/entity.cpp $(IntermediateDirectory)/src_framework_entity.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/entity.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_entity.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_entity.cpp$(DependSuffix): src/framework/entity.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_entity.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_entity.cpp$(DependSuffix) -MM src/framework/entity.cpp

$(IntermediateDirectory)/src_framework_entity.cpp$(PreprocessSuffix): src/framework/entity.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_entity.cpp$(PreprocessSuffix) src/framework/entity.cpp

$(IntermediateDirectory)/src_entities_mongus.cpp$(ObjectSuffix): src/entities/mongus.cpp $(IntermediateDirectory)/src_entities_mongus.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/mongus.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_mongus.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_mongus.cpp$(DependSuffix): src/entities/mongus.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_mongus.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_mongus.cpp$(DependSuffix) -MM src/entities/mongus.cpp

$(IntermediateDirectory)/src_entities_mongus.cpp$(PreprocessSuffix): src/entities/mongus.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_mongus.cpp$(PreprocessSuffix) src/entities/mongus.cpp

$(IntermediateDirectory)/src_framework_logging.cpp$(ObjectSuffix): src/framework/logging.cpp $(IntermediateDirectory)/src_framework_logging.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/logging.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_logging.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_logging.cpp$(DependSuffix): src/framework/logging.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_logging.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_logging.cpp$(DependSuffix) -MM src/framework/logging.cpp

$(IntermediateDirectory)/src_framework_logging.cpp$(PreprocessSuffix): src/framework/logging.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_logging.cpp$(PreprocessSuffix) src/framework/logging.cpp

$(IntermediateDirectory)/src_framework_stats.cpp$(ObjectSuffix): src/framework/stats.cpp $(IntermediateDirectory)/src_framework_stats.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/stats.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_stats.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_stats.cpp$(DependSuffix): src/framework/stats.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_stats.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_stats.cpp$(DependSuffix) -MM src/framework/stats.cpp

$(IntermediateDirectory)/src_framework_stats.cpp$(PreprocessSuffix): src/framework/stats.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_stats.cpp$(PreprocessSuffix) src/framework/stats.cpp

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

$(IntermediateDirectory)/src_audio_sound.cpp$(ObjectSuffix): src/audio/sound.cpp $(IntermediateDirectory)/src_audio_sound.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/audio/sound.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_audio_sound.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_audio_sound.cpp$(DependSuffix): src/audio/sound.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_audio_sound.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_audio_sound.cpp$(DependSuffix) -MM src/audio/sound.cpp

$(IntermediateDirectory)/src_audio_sound.cpp$(PreprocessSuffix): src/audio/sound.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_audio_sound.cpp$(PreprocessSuffix) src/audio/sound.cpp

$(IntermediateDirectory)/src_framework_system.cpp$(ObjectSuffix): src/framework/system.cpp $(IntermediateDirectory)/src_framework_system.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/system.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_system.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_system.cpp$(DependSuffix): src/framework/system.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_system.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_system.cpp$(DependSuffix) -MM src/framework/system.cpp

$(IntermediateDirectory)/src_framework_system.cpp$(PreprocessSuffix): src/framework/system.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_system.cpp$(PreprocessSuffix) src/framework/system.cpp

$(IntermediateDirectory)/src_framework_event.cpp$(ObjectSuffix): src/framework/event.cpp $(IntermediateDirectory)/src_framework_event.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/framework/event.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_framework_event.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_framework_event.cpp$(DependSuffix): src/framework/event.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_framework_event.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_framework_event.cpp$(DependSuffix) -MM src/framework/event.cpp

$(IntermediateDirectory)/src_framework_event.cpp$(PreprocessSuffix): src/framework/event.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_framework_event.cpp$(PreprocessSuffix) src/framework/event.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(IntermediateDirectory)


