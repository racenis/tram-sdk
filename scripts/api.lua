-- TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
-- All rights reserved.

if tram then return end
tram = {}
tram.entity = {}
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

-- FRAMEWORK/ENTITY.H

function tram.entity._make(id)
	entity = {}
	entity.id = id
	
	entity.GetName = function (self)
		return __tram_impl_entity_get_name(self.id)
	end
	
	entity.GetLocation = function (self)
		return __tram_impl_entity_get_location(self.id)
	end
	
	entity.SetLocation = function (self, location)
		return __tram_impl_entity_set_location(self.id, location)
	end
	
	return entity
end

function tram.entity.Find(term)
	if type(term) == "string" then
		id = __tram_impl_entity_find_by_name(term)
	else
		id = __tram_impl_entity_find_by_id(term)
	end

	if id == 0 then
		return nil
	end
	
	return tram.entity._make(id)
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

tram.event._evt_act = {}
tram.event._evt_ids = 1

function tram.event.AddListener(event_type, listener_action)
	listener = __tram_impl_event_add_listener(event_type, tram.event._evt_ids)

	tram.event._evt_act[tram.event._evt_ids] = listener_action
	tram.event._evt_ids = tram.event._evt_ids + 1
	
	return listener
end

function tram.event.RemoveListener(listener)
	-- TODO: delete from _evt_act also
	__tram_impl_event_remove_listener(listener)
end

function __tram_impl_event_event_callback(event_type, event_subtype, event_poster, event_data, callback_data)
	event = {}
	event.type = event_type
	event.subtype = event_subtype
	event.poster = event_poster
	event.data = event_data
	
	callback = tram.event._evt_act[callback_data]
	
	callback(event)
end

tram.event.KEYPRESS = tram.event.GetType("keypress")
tram.event.KEYDOWN = tram.event.GetType("keydown")
tram.event.KEYUP = tram.event.GetType("keyup")
tram.event.KEYCHAR = tram.event.GetType("keychar")
tram.event.CURSORPOS = tram.event.GetType("cursorpos")
tram.event.TICK = tram.event.GetType("tick")
tram.event.SELECTED = tram.event.GetType("selected")
tram.event.LOOK_AT = tram.event.GetType("look-at")
