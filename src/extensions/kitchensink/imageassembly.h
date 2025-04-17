// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_EXTENSIONS_KITCHENSINK_IMAGEASSEMBLY_H
#define TRAM_SDK_EXTENSIONS_KITCHENSINK_IMAGEASSEMBLY_H

#include <render/raytrace/raytrace.h>

namespace tram::Kitchensink::ImageAssembly {
    
void Add(Render::API::AssemblyLayers layers);
void Reset();

void Save(const char* filename);

}

#endif // TRAM_SDK_EXTENSIONS_KITCHENSINK_IMAGEASSEMBLY_H