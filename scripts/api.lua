-- TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
-- All rights reserved.

if tram then return end
tram = {}
tram.event = {}

-- FRAMEWORK/CORE.H

function tram.init()
	print "hello my big chungulina"
end

function tram.GetTick()
	return __tram_impl_core_get_tick()
end

function tram.GetTickTime()
	return __tram_impl_core_get_tick_time()
end

function tram.GetDeltaTime()
	return __tram_impl_core_get_delta_time()
end


-- FRAMEWORK/EVENT.H

function tram.event.Register(event_name)
	return __tram_impl_event_register(event_name)
end

function tram.event.GetType(event_name)
	return __tram_impl_event_get_type(event_name)
end

function tram.event.GetName(event_name)
	return __tram_impl_event_get_name(event_name)
end

function tram.event.GetLast()
	return __tram_impl_event_get_last()
end

function tram.event.Post(event_type, event_subtype, poster, data)
	__tram_impl_event_post(event_type, event_subtype, post, data)
end

tram.event.KEYPRESS = tram.event.GetType("keypress")
tram.event.KEYDOWN = tram.event.GetType("keydown")
tram.event.KEYUP = tram.event.GetType("keyup")
tram.event.KEYCHAR = tram.event.GetType("keychar")
tram.event.CURSORPOS = tram.event.GetType("cursorpos")
tram.event.TICK = tram.event.GetType("tick")
tram.event.SELECTED = tram.event.GetType("selected")
tram.event.LOOK_AT = tram.event.GetType("look-at")
