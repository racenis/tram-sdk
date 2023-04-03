#include <components/lightcomponent.h>

namespace tram {

template <> Pool<LightComponent> PoolProxy<LightComponent>::pool ("light component pool", 250, false);   

}