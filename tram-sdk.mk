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
Date                   :=29/07/2022
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
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)./src $(IncludeSwitch)libraries 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)BulletSoftBody $(LibrarySwitch)BulletDynamics $(LibrarySwitch)BulletCollision $(LibrarySwitch)LinearMath $(LibrarySwitch)glfw3 
ArLibs                 :=  "libBulletSoftBody.a" "libBulletDynamics.a" "libBulletCollision.a" "libLinearMath.a" "libglfw3.a" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)libraries 

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
Objects0=$(IntermediateDirectory)/src_entities_bus.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_crate.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_lamp.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_net.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_staticworldobject.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_material.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_player.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_shader.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_render_misc.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_async.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_worldcell.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_physics.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_glyphs.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entitycomponents.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_render_model.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_mongus.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_core.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entity.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_ui.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_entities_tram.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: MakeIntermediateDirs $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@if not exist "$(IntermediateDirectory)" $(MakeDirCommand) "$(IntermediateDirectory)"
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
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
$(IntermediateDirectory)/src_entities_bus.cpp$(ObjectSuffix): src/entities/bus.cpp $(IntermediateDirectory)/src_entities_bus.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/bus.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_bus.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_bus.cpp$(DependSuffix): src/entities/bus.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_bus.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_bus.cpp$(DependSuffix) -MM src/entities/bus.cpp

$(IntermediateDirectory)/src_entities_bus.cpp$(PreprocessSuffix): src/entities/bus.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_bus.cpp$(PreprocessSuffix) src/entities/bus.cpp

$(IntermediateDirectory)/src_entities_crate.cpp$(ObjectSuffix): src/entities/crate.cpp $(IntermediateDirectory)/src_entities_crate.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/crate.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_crate.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_crate.cpp$(DependSuffix): src/entities/crate.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_crate.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_crate.cpp$(DependSuffix) -MM src/entities/crate.cpp

$(IntermediateDirectory)/src_entities_crate.cpp$(PreprocessSuffix): src/entities/crate.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_crate.cpp$(PreprocessSuffix) src/entities/crate.cpp

$(IntermediateDirectory)/src_entities_lamp.cpp$(ObjectSuffix): src/entities/lamp.cpp $(IntermediateDirectory)/src_entities_lamp.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/lamp.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_lamp.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_lamp.cpp$(DependSuffix): src/entities/lamp.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_lamp.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_lamp.cpp$(DependSuffix) -MM src/entities/lamp.cpp

$(IntermediateDirectory)/src_entities_lamp.cpp$(PreprocessSuffix): src/entities/lamp.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_lamp.cpp$(PreprocessSuffix) src/entities/lamp.cpp

$(IntermediateDirectory)/src_net.cpp$(ObjectSuffix): src/net.cpp $(IntermediateDirectory)/src_net.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/net.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_net.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_net.cpp$(DependSuffix): src/net.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_net.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_net.cpp$(DependSuffix) -MM src/net.cpp

$(IntermediateDirectory)/src_net.cpp$(PreprocessSuffix): src/net.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_net.cpp$(PreprocessSuffix) src/net.cpp

$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(ObjectSuffix): src/entities/staticworldobject.cpp $(IntermediateDirectory)/src_entities_staticworldobject.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/staticworldobject.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(DependSuffix): src/entities/staticworldobject.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(DependSuffix) -MM src/entities/staticworldobject.cpp

$(IntermediateDirectory)/src_entities_staticworldobject.cpp$(PreprocessSuffix): src/entities/staticworldobject.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_staticworldobject.cpp$(PreprocessSuffix) src/entities/staticworldobject.cpp

$(IntermediateDirectory)/src_render.cpp$(ObjectSuffix): src/render.cpp $(IntermediateDirectory)/src_render.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render.cpp$(DependSuffix): src/render.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render.cpp$(DependSuffix) -MM src/render.cpp

$(IntermediateDirectory)/src_render.cpp$(PreprocessSuffix): src/render.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render.cpp$(PreprocessSuffix) src/render.cpp

$(IntermediateDirectory)/src_render_material.cpp$(ObjectSuffix): src/render/material.cpp $(IntermediateDirectory)/src_render_material.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/material.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_material.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_material.cpp$(DependSuffix): src/render/material.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_material.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_material.cpp$(DependSuffix) -MM src/render/material.cpp

$(IntermediateDirectory)/src_render_material.cpp$(PreprocessSuffix): src/render/material.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_material.cpp$(PreprocessSuffix) src/render/material.cpp

$(IntermediateDirectory)/src_entities_player.cpp$(ObjectSuffix): src/entities/player.cpp $(IntermediateDirectory)/src_entities_player.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/player.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_player.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_player.cpp$(DependSuffix): src/entities/player.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_player.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_player.cpp$(DependSuffix) -MM src/entities/player.cpp

$(IntermediateDirectory)/src_entities_player.cpp$(PreprocessSuffix): src/entities/player.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_player.cpp$(PreprocessSuffix) src/entities/player.cpp

$(IntermediateDirectory)/src_shader.cpp$(ObjectSuffix): src/shader.cpp $(IntermediateDirectory)/src_shader.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/shader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_shader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_shader.cpp$(DependSuffix): src/shader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_shader.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_shader.cpp$(DependSuffix) -MM src/shader.cpp

$(IntermediateDirectory)/src_shader.cpp$(PreprocessSuffix): src/shader.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_shader.cpp$(PreprocessSuffix) src/shader.cpp

$(IntermediateDirectory)/src_render_render_misc.cpp$(ObjectSuffix): src/render/render_misc.cpp $(IntermediateDirectory)/src_render_render_misc.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/render_misc.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_render_misc.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_render_misc.cpp$(DependSuffix): src/render/render_misc.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_render_misc.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_render_misc.cpp$(DependSuffix) -MM src/render/render_misc.cpp

$(IntermediateDirectory)/src_render_render_misc.cpp$(PreprocessSuffix): src/render/render_misc.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_render_misc.cpp$(PreprocessSuffix) src/render/render_misc.cpp

$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix): src/main.cpp $(IntermediateDirectory)/src_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main.cpp$(DependSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main.cpp$(DependSuffix) -MM src/main.cpp

$(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix): src/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix) src/main.cpp

$(IntermediateDirectory)/src_async.cpp$(ObjectSuffix): src/async.cpp $(IntermediateDirectory)/src_async.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/async.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_async.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_async.cpp$(DependSuffix): src/async.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_async.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_async.cpp$(DependSuffix) -MM src/async.cpp

$(IntermediateDirectory)/src_async.cpp$(PreprocessSuffix): src/async.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_async.cpp$(PreprocessSuffix) src/async.cpp

$(IntermediateDirectory)/src_worldcell.cpp$(ObjectSuffix): src/worldcell.cpp $(IntermediateDirectory)/src_worldcell.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/worldcell.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_worldcell.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_worldcell.cpp$(DependSuffix): src/worldcell.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_worldcell.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_worldcell.cpp$(DependSuffix) -MM src/worldcell.cpp

$(IntermediateDirectory)/src_worldcell.cpp$(PreprocessSuffix): src/worldcell.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_worldcell.cpp$(PreprocessSuffix) src/worldcell.cpp

$(IntermediateDirectory)/src_physics.cpp$(ObjectSuffix): src/physics.cpp $(IntermediateDirectory)/src_physics.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/physics.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_physics.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_physics.cpp$(DependSuffix): src/physics.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_physics.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_physics.cpp$(DependSuffix) -MM src/physics.cpp

$(IntermediateDirectory)/src_physics.cpp$(PreprocessSuffix): src/physics.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_physics.cpp$(PreprocessSuffix) src/physics.cpp

$(IntermediateDirectory)/src_render_glyphs.cpp$(ObjectSuffix): src/render/glyphs.cpp $(IntermediateDirectory)/src_render_glyphs.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/glyphs.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_glyphs.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_glyphs.cpp$(DependSuffix): src/render/glyphs.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_glyphs.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_glyphs.cpp$(DependSuffix) -MM src/render/glyphs.cpp

$(IntermediateDirectory)/src_render_glyphs.cpp$(PreprocessSuffix): src/render/glyphs.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_glyphs.cpp$(PreprocessSuffix) src/render/glyphs.cpp

$(IntermediateDirectory)/src_entitycomponents.cpp$(ObjectSuffix): src/entitycomponents.cpp $(IntermediateDirectory)/src_entitycomponents.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entitycomponents.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entitycomponents.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entitycomponents.cpp$(DependSuffix): src/entitycomponents.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entitycomponents.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entitycomponents.cpp$(DependSuffix) -MM src/entitycomponents.cpp

$(IntermediateDirectory)/src_entitycomponents.cpp$(PreprocessSuffix): src/entitycomponents.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entitycomponents.cpp$(PreprocessSuffix) src/entitycomponents.cpp

$(IntermediateDirectory)/src_render_model.cpp$(ObjectSuffix): src/render/model.cpp $(IntermediateDirectory)/src_render_model.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/render/model.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_render_model.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_render_model.cpp$(DependSuffix): src/render/model.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_render_model.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_render_model.cpp$(DependSuffix) -MM src/render/model.cpp

$(IntermediateDirectory)/src_render_model.cpp$(PreprocessSuffix): src/render/model.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_render_model.cpp$(PreprocessSuffix) src/render/model.cpp

$(IntermediateDirectory)/src_entities_mongus.cpp$(ObjectSuffix): src/entities/mongus.cpp $(IntermediateDirectory)/src_entities_mongus.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/mongus.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_mongus.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_mongus.cpp$(DependSuffix): src/entities/mongus.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_mongus.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_mongus.cpp$(DependSuffix) -MM src/entities/mongus.cpp

$(IntermediateDirectory)/src_entities_mongus.cpp$(PreprocessSuffix): src/entities/mongus.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_mongus.cpp$(PreprocessSuffix) src/entities/mongus.cpp

$(IntermediateDirectory)/src_core.cpp$(ObjectSuffix): src/core.cpp $(IntermediateDirectory)/src_core.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/core.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_core.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_core.cpp$(DependSuffix): src/core.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_core.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_core.cpp$(DependSuffix) -MM src/core.cpp

$(IntermediateDirectory)/src_core.cpp$(PreprocessSuffix): src/core.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_core.cpp$(PreprocessSuffix) src/core.cpp

$(IntermediateDirectory)/src_entity.cpp$(ObjectSuffix): src/entity.cpp $(IntermediateDirectory)/src_entity.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entity.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entity.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entity.cpp$(DependSuffix): src/entity.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entity.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entity.cpp$(DependSuffix) -MM src/entity.cpp

$(IntermediateDirectory)/src_entity.cpp$(PreprocessSuffix): src/entity.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entity.cpp$(PreprocessSuffix) src/entity.cpp

$(IntermediateDirectory)/src_ui.cpp$(ObjectSuffix): src/ui.cpp $(IntermediateDirectory)/src_ui.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/ui.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_ui.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_ui.cpp$(DependSuffix): src/ui.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_ui.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_ui.cpp$(DependSuffix) -MM src/ui.cpp

$(IntermediateDirectory)/src_ui.cpp$(PreprocessSuffix): src/ui.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_ui.cpp$(PreprocessSuffix) src/ui.cpp

$(IntermediateDirectory)/src_entities_tram.cpp$(ObjectSuffix): src/entities/tram.cpp $(IntermediateDirectory)/src_entities_tram.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/Poga/Desktop/painis/tram-sdk/src/entities/tram.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_entities_tram.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_entities_tram.cpp$(DependSuffix): src/entities/tram.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_entities_tram.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_entities_tram.cpp$(DependSuffix) -MM src/entities/tram.cpp

$(IntermediateDirectory)/src_entities_tram.cpp$(PreprocessSuffix): src/entities/tram.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_entities_tram.cpp$(PreprocessSuffix) src/entities/tram.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(IntermediateDirectory)


