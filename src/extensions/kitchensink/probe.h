// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_EXTENSIONS_KITCHENSINK_PROBE_H
#define TRAM_SDK_EXTENSIONS_KITCHENSINK_PROBE_H

#include <framework/worldcell.h>

namespace tram::Ext::Kitchensink {

void RegisterRenderEnvironmentMapShortcut();
void RenderEnvironmentMaps(WorldCell*);

}

#endif // TRAM_SDK_EXTENSIONS_KITCHENSINK_PROBE_H