#include <components/playercomponent.h>

namespace tram {

template <> Pool<PlayerComponent> PoolProxy<PlayerComponent>::pool ("player component pool", 5, false);

}