-- TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
-- All rights reserved.

if tram then return end
tram = {}
tram.math = {}
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



-- ============================ FRAMEWORK/MATH.H ============================ --
-- This is where all of the vector math stuff is defined.
-- For simple stuff, we just do all of the computation without leaving Lua. For
-- more complex stuff we go down into C++.


-- -------------------------------  VEC3 TYPE ------------------------------- --
-- The vec3 type is basically a re-implemenation of tram::vec3, which itself is
-- just an alias for glm::vec3.

-- To create a new vec3, use tram.math.vec3(x, y, z). If you get a vec3 through
-- the C++ API, then use setmetatable(object, tram.math._metatable_vec3) to set
-- its metatable.

tram.math._methods_vec3 = {
	poot = function(self)
		--value = "(" + self.x + self.y + self.z + ")"
		value = string.format("(%.4f, %.4f, %.4f)", self.x, self.y, self.z)
		print(value)
		--print("vec3:", self.x, self.y, self.z)
	end
}

tram.math._metatable_vec3 = {
	__index = tram.math._methods_vec3,
	
	__tostring = function(self)
		return string.format("(%.4f, %.4f, %.4f)", self.x, self.y, self.z)
	end,
	
	__eq = function(self, other)
		return self.x == other.x and self.y == other.y and self.z == other.z
	end
}

function tram.math.vec3(x, y, z)
	vector = {}
	vector.x = x
	vector.y = y
	vector.z = z
	
	setmetatable(vector, tram.math._metatable_vec3)
	
	return vector
	
	--return tram.math._make_vec3(vector)
end







-- -------------------------------  QUAT TYPE ------------------------------- --
-- The quat type is basically a re-implemenation of tram::quat, which itself is
-- just an alias for glm::quat.

-- To create a new quat, use tram.math.quat(x, y, z, w). You can also use the
-- tram.math.quat(vec3) to construct a quaternion from euler angles contained in
-- the vec3. If you get a quat through the C++ API, then use
-- setmetatable(object, tram.math._metatable_quat) to set its metatable.

tram.math._methods_quat = {
	poot = function(self)
		value = string.format("(%.4f, %.4f, %.4f)", self.x, self.y, self.z)
		print(value)
	end
}

tram.math._metatable_quat = {
	__index = tram.math._methods_quat,
	
	__tostring = function(self)
		return string.format("(%.4f, %.4f, %.4f, %.4f)", self.x, self.y, self.z, self.w)
	end,
	
	__eq = function(self, other)
		return self.x == other.x and self.y == other.y and self.z == other.z and self.w == other.w
	end,
	
	__mul = function(self, other)
		assert(getmetatable(self) == tram.math._metatable_quat)
		
		local result = {}
		
		if getmetatable(other) == tram.math._metatable_quat then
			result.w = other.w * self.w - other.x * self.x - other.y * self.y - other.z * self.z
			result.x = other.w * self.x + other.x * self.w - other.y * self.z + other.z * self.y
			result.y = other.w * self.y + other.x * self.z + other.y * self.w - other.z * self.x
			result.z = other.w * self.z - other.x * self.y + other.y * self.x + other.z * self.w
		else if getmetatable(other) == tram.math._metatable_vec3 then
			result = __tram_impl_math_quat_vec3_multiply(self, other)
		else
			error("quaternion invalid rotation")
		end
		
		setmetatable(result, tram.math._metatable_quat)
	
		return result
	end
}

function tram.math.quat(x, y, z, w)
	local vector = {}
	
	-- conversion from euler angles
	if getmetatable(x) == tram.math._metatable_vec3 then
		local roll = x.x
		local pitch = x.y
		local yaw = x.z
		
		local cr = math.cos(roll * 0.5);
		local sr = math.sin(roll * 0.5);
		local cp = math.cos(pitch * 0.5);
		local sp = math.sin(pitch * 0.5);
		local cy = math.cos(yaw * 0.5);
		local sy = math.sin(yaw * 0.5);

		vector.w = cr * cp * cy + sr * sp * sy;
		vector.x = sr * cp * cy - cr * sp * sy;
		vector.y = cr * sp * cy + sr * cp * sy;
		vector.z = cr * cp * sy - sr * sp * cy;
		
	-- normal parametric constructor
	elseif x ~= nil and y ~= nil and z ~= nil and w ~= nil then
		vector.x = x
		vector.y = y
		vector.z = z
		vector.w = w
	else
		error("Something is wrong with quaternion constructor parameters!")
	end
	
	setmetatable(vector, tram.math._metatable_quat)
	
	return vector
end



-- FRAMEWORK/ENTITY.H

function tram.entity._make(id)
	entity = {}
	entity.id = id
	
	
	entity.GetName = function (self)
		return __tram_impl_entity_get_name(self.id)
	end
	
	entity.GetID = function (self)
		return self.id
	end
	
	
	
	entity.GetLocation = function (self)
		vector = __tram_impl_entity_get_location(self.id)
		setmetatable(vector, tram.math._metatable_vec3)
		return vector
	end
	entity.SetLocation = function (self, location)
		return __tram_impl_entity_set_location(self.id, location)
	end
	
	entity.GetRotation = function (self)
		vector = __tram_impl_entity_get_rotation(self.id)
		setmetatable(vector, tram.math._metatable_quat)
		return vector
	end
	entity.SetRotation = function (self, rotation)
		return __tram_impl_entity_set_rotation(self.id, rotation)
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
