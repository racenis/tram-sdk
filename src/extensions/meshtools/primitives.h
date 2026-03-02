// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_EXTENSIONS_MESHTOOLS_PRIMITIVES_H
#define TRAM_SDK_EXTENSIONS_MESHTOOLS_PRIMITIVES_H

#include <components/mesh.h>

namespace tram::Ext::Meshtools {
    
void MakeCubeSphere(MeshComponent& mesh, size_t subdivisions, float radius);

}

#endif // TRAM_SDK_EXTENSIONS_MESHTOOLS_PRIMITIVES_H