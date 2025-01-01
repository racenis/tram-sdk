-- TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
-- All rights reserved.

print("\n\nLoading Lua API")

if tram then return end
tram = {}
tram.math = {}
tram.entity = {}
tram.event = {}
tram.message = {}
tram.type = {}
tram.ui = {}
tram.worldcell = {}
tram.audio = {}
tram.render = {}
tram.physics = {}
tram.physics.shape = {}
tram.physics.model = {}
tram.resource = {}
tram.components = {}

tram.render.animation = {}
tram.render.model = {}
tram.render.material = {}
tram.render.sprite = {}



-- FRAMEWORK/CORE.H

function tram.init()
	print "hello my big chungulina"
end

function tram.GetTick()
	return __impl_core_get_tick()
end

function tram.GetTickTime()
	return __impl_core_get_tick_time()
end

function tram.GetDeltaTime()
	return __impl_core_get_delta_time()
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
	end,
	
	length = function(self)
		return math.sqrt(self.x * self.x + self.y * self.y + self.z * self.z)
	end,
	
	distance = function(self, other)
		assert(getmetatable(other) == tram.math._metatable_vec3)
		return (self - other):length()
	end,
	
	dot = function(self, other)
		assert(getmetatable(other) == tram.math._metatable_vec3)
		return self.x * other.x + self.y * other.y + self.z * other.z
	end,
	
	cross = function(self, other)
		assert(getmetatable(other) == tram.math._metatable_vec3)
		
		local result = {}
		
		result.x = self.y * other.z - self.z * other.y
		result.y = self.z * other.x - self.x * other.z
		result.z = self.x * other.y - self.y * other.x
		
		setmetatable(result, tram.math._metatable_vec3)
		
		return result
	end,
	
	normalize = function(self)
		local result = {}
		local length = self:length()
		
		result.x = self.x / length
		result.y = self.y / length
		result.z = self.z / length
		
		setmetatable(result, tram.math._metatable_vec3)
		
		return result
	end,
}

tram.math._metatable_vec3 = {
	__index = tram.math._methods_vec3,
	
	__tostring = function(self)
		return string.format("(%.4f, %.4f, %.4f)", self.x, self.y, self.z)
	end,
	
	__eq = function(self, other)
		return self.x == other.x and self.y == other.y and self.z == other.z
	end,
	
	__add = function(self, other)
		local result = {}
		
		if getmetatable(other) == tram.math._metatable_vec3 then
			result.x = self.x + other.x
			result.y = self.y + other.y
			result.z = self.z + other.z
		else
			result.x = self.x + other
			result.y = self.y + other
			result.z = self.z + other
		end
	
		setmetatable(result, tram.math._metatable_vec3)
	
		return result
	end,
	
	__sub = function(self, other)
		local result = {}
		
		if getmetatable(other) == tram.math._metatable_vec3 then
			result.x = self.x - other.x
			result.y = self.y - other.y
			result.z = self.z - other.z
		else
			result.x = self.x - other
			result.y = self.y - other
			result.z = self.z - other
		end
	
		setmetatable(result, tram.math._metatable_vec3)
	
		return result
	end,
	
	__mul = function(self, other)
		local result = {}
		
		if getmetatable(other) == tram.math._metatable_vec3 then
			result.x = self.x * other.x
			result.y = self.y * other.y
			result.z = self.z * other.z
		else
			result.x = self.x * other
			result.y = self.y * other
			result.z = self.z * other
		end
	
		setmetatable(result, tram.math._metatable_vec3)
	
		return result
	end,
	
	__div = function(self, other)
		local result = {}
		
		if getmetatable(other) == tram.math._metatable_vec3 then
			result.x = self.x / other.x
			result.y = self.y / other.y
			result.z = self.z / other.z
		else
			result.x = self.x / other
			result.y = self.y / other
			result.z = self.z / other
		end
	
		setmetatable(result, tram.math._metatable_vec3)
	
		return result
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
		elseif getmetatable(other) == tram.math._metatable_vec3 then
			result = __impl_math_quat_vec3_multiply(self, other)
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

-- -------------------------------  CONSTANTS ------------------------------- --

tram.math.DIRECTION_FORWARD = tram.math.vec3(0.0, 0.0, -1.0)
tram.math.DIRECTION_SIDE = tram.math.vec3(1.0, 0.0, 0.0)
tram.math.DIRECTION_UP = tram.math.vec3(0.0, 1.0, 0.0)

-- FRAMEWORK/ENTITY.H

tram._metatable_entity = {
	__index = {
		GetName = function (self)
			return __impl_entity_get_name(self.id)
		end,
		GetID = function (self)
			return self.id
		end,
		
		Load = function (self)
			__impl_entity_load(self.id)
		end,
		Unload = function (self)
			__impl_entity_unload(self.id)
		end,
		
		GetLocation = function (self)
			local vector = __impl_entity_get_location(self.id)
			setmetatable(vector, tram.math._metatable_vec3)
			return vector
		end,
		SetLocation = function (self, location)
			return __impl_entity_set_location(self.id, location)
		end,
		
		GetRotation = function (self)
			local vector = __impl_entity_get_rotation(self.id)
			setmetatable(vector, tram.math._metatable_quat)
			return vector
		end,
		SetRotation = function (self, rotation)
			return __impl_entity_set_rotation(self.id, rotation)
		end
	}
}

function tram.entity.Find(term)
	if type(term) == "string" then
		id = __impl_entity_find_by_name(term)
	else
		id = __impl_entity_find_by_id(term)
	end

	if id == 0 then
		return nil
	end
	
	local entity = {}
	entity.id = id
	
	setmetatable(entity, tram._metatable_entity)
	
	return entity
end

function tram.entity.Make(entity_type, entity_properties)
	
	__impl_clear_key_value()
	
	for key, value in pairs(entity_properties) do
		if type(value) ~= "function" then
			__impl_push_key_value(key, value)
		end
	end
	
	local entity = {}
	
	entity.id = __impl_entity_make(entity_type)
	
	setmetatable(entity, tram._metatable_entity)
	
	return entity
end

tram.entity._scriptable_entities = {}
tram.entity._scriptable_entity_constructor = {}
tram.entity._scriptable_entity_destructor = {}

function __impl_entity_update_parameters_callback(id)
	local entity = tram.entity._scriptable_entities[id]
	
	assert(entity ~= nil)
	
	if (rawget(entity, "OnUpdateParameters") == nil) then
		return true
	end

	local result = entity:OnUpdateParameters()
	
	if result == nil then
		return true
	end
	
	return result
end

function __impl_entity_set_parameters_callback(id)
	local entity = tram.entity._scriptable_entities[id]
	
	assert(entity ~= nil)
	
	if (rawget(entity, "OnSetParameters") == nil) then
		return true
	end

	local result = entity:OnSetParameters()
	
	if result == nil then
		return true
	end
	
	return result
end

function __impl_entity_load_callback(id)
	local entity = tram.entity._scriptable_entities[id]
	
	assert(entity ~= nil)
	
	if (rawget(entity, "OnLoad") == nil) then
		return true
	end

	local result = entity:OnLoad()
	
	if result == nil then
		return true
	end
	
	return result
end

function __impl_entity_unload_callback(id)
	local entity = tram.entity._scriptable_entities[id]
	
	assert(entity ~= nil)
	
	if (rawget(entity, "OnUnload") == nil) then
		return true
	end

	local result = entity:OnUnload()
	
	if result == nil then
		return true
	end
	
	return result
end

function __impl_entity_message_handler_callback(id, message_type, sender, receiver, data)
	local entity = tram.entity._scriptable_entities[id]
	
	assert(entity ~= nil)
	
	if (entity.OnMessageHandler == nil) then
		return true
	end
	
	local message = {}
	message.type = message_type
	message.sender = sender
	message.receiver = receiver
	message.data = data	

	local result = entity:OnMessageHandler(message)
	
	if result == nil then
		return true
	end
	
	return result
end

function __impl_entity_event_handler_callback(id, event_type, subtype, poster, data)
	local entity = tram.entity._scriptable_entities[id]
	
	assert(entity ~= nil)
	
	if (entity.OnEventHandler == nil) then
		return true
	end
	
	local event = {}
	event.type = event_type
	event.subtype = subtype
	event.poster = poster
	event.data = data

	local result = entity:OnEventHandler(event)
	
	if result == nil then
		return true
	end
	
	return result
end


function tram.entity.New(entity_type, base_type, base_properties)
	__impl_clear_key_value()
	
	for key, value in pairs(base_properties) do
		if type(value) ~= "function" then
			__impl_push_key_value(key, value)
		end
	end
	
	local entity = {}
	
	entity.id = __impl_entity_scriptable_make(base_type, entity_type)
	
	setmetatable(entity, tram._metatable_entity)
	
	tram.entity._scriptable_entities[entity.id] = entity
	
	return entity
end



function tram.entity.Register(entity_type, entity_properties, constructor, destructor)
	__impl_clear_entity_fields()
	
	for index, field in pairs(entity_properties) do
		if type(index) ~= "number" then
			error("Property index needs to be a number, instead got"
				  .. type(index)
				  .. " (" .. type(index) .. ")")
		elseif type(field) ~= "table" then
			error("Property field needs to be a table, instead got"
				  .. type(field)
				  .. " (" .. type(field) .. ")")
		elseif type(field.type) ~= "number" then
			error("Property type needs to be a number, instead got"
				  .. type(field.type)
				  .. " (" .. type(field.type) .. ")")
		elseif type(field.flag) ~= "number" then
			error("Property type needs to be a number, instead got"
				  .. type(field.flag)
				  .. " (" .. type(field.flag) .. ")")
		else
			__impl_push_entity_fields(index, field.type, field.flag)
		end
	end
	
	tram.entity._scriptable_entity_constructor[entity_type] = constructor
	tram.entity._scriptable_entity_destructor[entity_type] = destructor
	
	__impl_entity_type_register(entity_type)
end



function __impl_entity_shared_data_callback(id, name, flags, location, rotation)
	tram.entity._entity_data_holder = {}

	tram.entity._entity_data_holder.id = id
	tram.entity._entity_data_holder.name = name
	tram.entity._entity_data_holder.flags = flags
	tram.entity._entity_data_holder.location = location
	tram.entity._entity_data_holder.rotation = rotation
end

function __impl_entity_property_callback(key, value)
	tram.entity._entity_data_holder[key] = value
end

function __impl_entity_constructor_callback(entity_type)
	local constructor = tram.entity._scriptable_entity_constructor[entity_type]
	assert(type(constructor) == "function")
	return constructor(tram.entity._entity_data_holder)
end

function __impl_entity_destructor_callback(entity_type, entity_id)
	local destructor = tram.entity._scriptable_entity_destructor[entity_type]
	assert(type(destructor) == "function")
	destructor(entity_id)
end

-- -------------------------------  CONSTANTS ------------------------------- --

-- TODO: fill in

-- =========================== FRAMEWORK/EVENT.H ============================ --

function tram.event.Register(event_name)
	return __impl_event_register(event_name)
end

function tram.event.GetType(event_name)
	return __impl_event_get_type(event_name)
end

function tram.event.GetName(event_name)
	return __impl_event_get_name(event_name)
end

function tram.event.GetLast()
	return __impl_event_get_last()
end

function tram.event.Post(event)
	__impl_event_post(event.type, event.subtype, event.poster, event.data)
end

-- this is dumb! we should instead use listener id as a key
-- instead of _evt_ids as a key!!!
tram.event._evt_act = {}
tram.event._evt_ids = 1

function tram.event.AddListener(event_type, action)
	if getmetatable(action) == tram._metatable_entity then
		return __impl_entity_add_listener(event_type, action.id)
	end
	
	listener = __impl_event_add_listener(event_type, tram.event._evt_ids)

	tram.event._evt_act[tram.event._evt_ids] = action
	tram.event._evt_ids = tram.event._evt_ids + 1
	
	return listener
end

function tram.event.RemoveListener(listener)
	-- TODO: delete from _evt_act also
	__impl_event_remove_listener(listener)
end

function __impl_event_event_callback(event_type, event_subtype, event_poster, event_data, callback_data)
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
tram.event.FRAME = tram.event.GetType("frame")
tram.event.SELECTED = tram.event.GetType("selected")
tram.event.LOOK_AT = tram.event.GetType("look-at")


-- ========================== FRAMEWORK/MESSAGE.H =========================== --

function tram.message.Register(message_name)
	return __impl_message_register(message_name)
end

function tram.message.GetType(message_name)
	return __impl_message_get_type(message_name)
end

function tram.message.GetName(message_name)
	return __impl_message_get_name(message_name)
end

function tram.message.GetLast()
	return __impl_message_get_last()
end

function tram.message.Send(message, delay)
	__impl_message_send(message.type, message.sender, message.receiver, message.data, delay)
end

tram.message.NONE = tram.message.GetType("none")
tram.message.PING = tram.message.GetType("ping")
tram.message.MOVE_PICK_UP = tram.message.GetType("move-pick-up")
tram.message.OPEN = tram.message.GetType("open")
tram.message.CLOSE = tram.message.GetType("close")
tram.message.LOCK = tram.message.GetType("lock")
tram.message.UNLOCK = tram.message.GetType("unlock")
tram.message.TOGGLE = tram.message.GetType("toggle")
tram.message.KILL = tram.message.GetType("kill")
tram.message.TRIGGER = tram.message.GetType("trigger")
tram.message.START = tram.message.GetType("start")
tram.message.STOP = tram.message.GetType("stop")
tram.message.ACTIVATE = tram.message.GetType("activate")
tram.message.ACTIVATE_ONCE = tram.message.GetType("activate-one")
tram.message.SELECT = tram.message.GetType("select")
tram.message.SET_PROGRESS = tram.message.GetType("set-progress")
tram.message.SET_ANIMATION = tram.message.GetType("set-animation")

-- ============================= FRAMEWORK/GUI.H ============================ --

-- TODO: implement

-- ========================== FRAMEWORK/LANGUAGE.H ========================== --

-- TODO: implement

-- ============================= FRAMEWORK/UI.H ============================= --

tram.ui.KEY_SPACE = 0
tram.ui.KEY_APOSTROPHE = 1
tram.ui.KEY_COMMA = 2
tram.ui.KEY_MINUS = 3
tram.ui.KEY_PERIOD = 4
tram.ui.KEY_SLASH = 5

tram.ui.KEY_0 = 6
tram.ui.KEY_1 = 7
tram.ui.KEY_2 = 8
tram.ui.KEY_3 = 9
tram.ui.KEY_4 = 10
tram.ui.KEY_5 = 11
tram.ui.KEY_6 = 12
tram.ui.KEY_7 = 13
tram.ui.KEY_8 = 14
tram.ui.KEY_9 = 15

tram.ui.KEY_SEMICOLON = 16
tram.ui.KEY_EQUAL = 17

tram.ui.KEY_A = 18
tram.ui.KEY_B = 19
tram.ui.KEY_C = 20
tram.ui.KEY_D = 21
tram.ui.KEY_E = 22
tram.ui.KEY_F = 23
tram.ui.KEY_G = 24
tram.ui.KEY_H = 25
tram.ui.KEY_I = 26
tram.ui.KEY_J = 27
tram.ui.KEY_K = 28
tram.ui.KEY_L = 29
tram.ui.KEY_M = 30
tram.ui.KEY_N = 31
tram.ui.KEY_O = 32
tram.ui.KEY_P = 33
tram.ui.KEY_Q = 34
tram.ui.KEY_R = 35
tram.ui.KEY_S = 36
tram.ui.KEY_T = 37
tram.ui.KEY_U = 38
tram.ui.KEY_V = 39
tram.ui.KEY_W = 40
tram.ui.KEY_X = 41
tram.ui.KEY_Y = 42
tram.ui.KEY_Z = 43

tram.ui.KEY_LEFT_BRACKET = 44
tram.ui.KEY_BACKSLASH = 45
tram.ui.KEY_RIGHT_BRACKET = 46
tram.ui.KEY_GRAVE_ACCENT = 47

tram.ui.KEY_WORLD_1 = 48
tram.ui.KEY_WORLD_2 = 49

tram.ui.KEY_ESCAPE = 50
tram.ui.KEY_ENTER = 51
tram.ui.KEY_TAB = 52
tram.ui.KEY_BACKSPACE = 53
tram.ui.KEY_INSERT = 54
tram.ui.KEY_DELETE = 55

tram.ui.KEY_RIGHT = 56
tram.ui.KEY_LEFT = 57
tram.ui.KEY_DOWN = 58
tram.ui.KEY_UP = 59

tram.ui.KEY_PAGE_UP = 60
tram.ui.KEY_PAGE_DOWN = 61
tram.ui.KEY_HOME = 62
tram.ui.KEY_END = 63
tram.ui.KEY_CAPS_LOCK = 64
tram.ui.KEY_SCROLL_LOCK = 65
tram.ui.KEY_NUM_LOCK = 66
tram.ui.KEY_PRINT_SCREEN = 67
tram.ui.KEY_PAUSE = 68

tram.ui.KEY_F1 = 69
tram.ui.KEY_F2 = 70
tram.ui.KEY_F3 = 71
tram.ui.KEY_F4 = 72
tram.ui.KEY_F5 = 73
tram.ui.KEY_F6 = 74
tram.ui.KEY_F7 = 75
tram.ui.KEY_F8 = 76
tram.ui.KEY_F9 = 77
tram.ui.KEY_F10 = 78
tram.ui.KEY_F11 = 79
tram.ui.KEY_F12 = 80
tram.ui.KEY_F13 = 81
tram.ui.KEY_F14 = 82
tram.ui.KEY_F15 = 83
tram.ui.KEY_F16 = 84
tram.ui.KEY_F17 = 85
tram.ui.KEY_F18 = 86
tram.ui.KEY_F19 = 87
tram.ui.KEY_F20 = 88
tram.ui.KEY_F21 = 89
tram.ui.KEY_F22 = 90
tram.ui.KEY_F23 = 91
tram.ui.KEY_F24 = 92
tram.ui.KEY_F25 = 93

tram.ui.KEY_KP_0 = 94
tram.ui.KEY_KP_1 = 95
tram.ui.KEY_KP_2 = 96
tram.ui.KEY_KP_3 = 97
tram.ui.KEY_KP_4 = 98
tram.ui.KEY_KP_5 = 99
tram.ui.KEY_KP_6 = 100
tram.ui.KEY_KP_7 = 101
tram.ui.KEY_KP_8 = 102
tram.ui.KEY_KP_9 = 103


tram.ui.KEY_KP_DECIMAL = 104
tram.ui.KEY_KP_DIVIDE = 105
tram.ui.KEY_KP_MULTIPLY = 106
tram.ui.KEY_KP_SUBTRACT = 107
tram.ui.KEY_KP_ADD = 108
tram.ui.KEY_KP_ENTER = 109
tram.ui.KEY_KP_EQUAL = 110

tram.ui.KEY_LEFT_SHIFT = 111
tram.ui.KEY_LEFT_CONTROL = 112
tram.ui.KEY_LEFT_ALT = 113
tram.ui.KEY_LEFT_SUPER = 114
tram.ui.KEY_RIGHT_SHIFT = 115
tram.ui.KEY_RIGHT_CONTROL = 116
tram.ui.KEY_RIGHT_ALT = 117
tram.ui.KEY_RIGHT_SUPER = 118

tram.ui.KEY_MENU = 119

tram.ui.KEY_RIGHTMOUSE = 120
tram.ui.KEY_LEFTMOUSE = 121
tram.ui.KEY_MIDDLEMOUSE = 122

tram.ui.KEY_MOUSE_X = 0
tram.ui.KEY_MOUSE_Y = 1
tram.ui.KEY_MOUSE_SCROLL = 2

tram.ui.STATE_DEFAULT = 0
tram.ui.STATE_NO_INPUT = 1
tram.ui.STATE_FLYING = 2
tram.ui.STATE_MENU_OPEN = 3
tram.ui.STATE_CURSOR = 4

tram.ui.KEY_ACTION_NONE = 0
tram.ui.KEY_ACTION_FORWARD = 1
tram.ui.KEY_ACTION_BACKWARD = 2
tram.ui.KEY_ACTION_STRAFE_LEFT = 3
tram.ui.KEY_ACTION_STRAFE_RIGHT = 4
tram.ui.KEY_ACTION_JUMP = 5
tram.ui.KEY_ACTION_CROUCH = 6
tram.ui.KEY_ACTION_SPRINT = 7
tram.ui.KEY_ACTION_ACTIVATE = 8
tram.ui.KEY_ACTION_UP = 9
tram.ui.KEY_ACTION_DOWN = 10
tram.ui.KEY_ACTION_LEFT = 11
tram.ui.KEY_ACTION_RIGHT = 12

tram.ui.CURSOR_DEFAULT = 12
tram.ui.CURSOR_TEXT = 12
tram.ui.CURSOR_CLICK = 12

function tram.ui.GetScreenWidth()
	return __impl_ui_get_screen_width()
end

function tram.ui.GetScreenHeight()
	return __impl_ui_get_screen_height()
end

function tram.ui.SetWindowTitle(title)
	__impl_ui_set_window_title(title)
end

function tram.ui.SetWindowSize(w, h)
	__impl_ui_set_window_size(w, h)
end

function tram.ui.SetCursor(cursor)
	__impl_ui_set_cursor(cursor)
end

tram.ui._keyboard_function = {}

function tram.ui.BindKeyboardKey(key, action)
	if type(action) == "function" then
		__impl_ui_bind_keyboard_callback(key)
		tram.ui._keyboard_function[key] = action
	else 
		__impl_ui_bind_keyboard_action(key, action)
	end
end

function __impl_ui_keyboard_callback(key)
	callback = tram.ui._keyboard_function[key]
	callback()
end

function tram.ui.PollKeyboardKey(key)
	return __impl_ui_poll_keyboard_key(key)
end

function tram.ui.PollKeyboardAxis(axis)
	return __impl_ui_poll_keyboard_axis(axis)
end

function tram.ui.PollKeyboardAxisDelta(axis)
	return __impl_ui_poll_keyboard_axis_delta(axis)
end

function tram.ui.SetInputState(state)
	__impl_ui_set_input_state(state)
end

function tram.ui.GetInputState()
	return __impl_ui_get_input_state()
end


function tram.ui.GetAxisSensitivity(axis)
	return __impl_ui_get_axis_sensitivity(axis)
end

function tram.ui.SetAxisSensitivity(axis, value)
	return __impl_ui_set_axis_sensitivity(axis, value)
end

function tram.ui.RegisterKeyboardAction(name)
	return __impl_ui_register_keyboard_action(name)
end

function tram.ui.GetKeyboardAction(name)
	return __impl_ui_get_keyboard_action(name)
end

function tram.ui.GetKeyboardActionName(name)
	return __impl_ui_get_keyboard_action_name(name)
end

-- ============================ FRAMEWORK/TYPE.H ============================ --

-- -------------------------------  CONSTANTS ------------------------------- --

tram.type.UNDEFINED = 0
tram.type.BOOL = 1
tram.type.NAME = 2
tram.type.STRING = 3
tram.type.INT8 = 4
tram.type.INT16 = 5
tram.type.INT32 = 6
tram.type.UINT8 = 7
tram.type.UINT16 = 8
tram.type.UINT32 = 9
tram.type.FLOAT32 = 10
tram.type.VEC2 = 11
tram.type.VEC3 = 12
tram.type.VEC4 = 13
tram.type.QUAT = 14

-- =========================== FRAMEWORK/RESOURCE.H ========================= --

-- -------------------------------  CONSTANTS ------------------------------- --

tram.resource.UNLOADED = 0
tram.resource.LOADED = 1
tram.resource.READY = 2

-- ========================== FRAMEWORK/WORLDCELL.H ========================= --

tram._metatable_worldcell = {
	__index = {
		GetName = function(self)
			return __impl_worldcell_get_name(self.index)
		end,
		
		LoadFromDisk = function(self)
			__impl_worldcell_loadfromdisk(self.index)
		end,
		
		Load = function(self)
			__impl_worldcell_load(self.index)
		end,
		
		Unload = function(self)
			__impl_worldcell_unload(self.index)
		end,
		
		Link = function(self, other)
			assert(getmetatable(other) == tram._metatable_worldcell)
			__impl_worldcell_link(self.index, other.index)
		end,
		
		IsLoaded = function(self)
			return __impl_worldcell_is_loaded(self.index)
		end,
		IsInterior = function(self)
			return __impl_worldcell_is_interior(self.index)
		end,
		HasInteriorLighting = function(self)
			return __impl_worldcell_has_interior_lighting(self.index)
		end,
		HasAutomaticLoading = function(self)
			return __impl_worldcell_has_automatic_loading(self.index)
		end,
		
		SetInterior = function(self, is)
			return __impl_worldcell_set_interior(self.index, is)
		end,
		SetInteriorLighting = function(self, is)
			return __impl_worldcell_set_interior_lighting(self.index, is)
		end,
		SetAutomaticLoading = function(self, is)
			return __impl_worldcell_set_automatic_loading(self.index, is)
		end
	}
}

function tram.worldcell.Make(name)
	local worldcell_index = __impl_worldcell_make(name)
	
	if (worldcell_index == -1) then
		return nil
	end
	
	local worldcell = {}
	worldcell.index = worldcell_index
	
	setmetatable(worldcell, tram._metatable_worldcell)
	
	return worldcell
end

function tram.worldcell.Find(name)
	local worldcell_index = __impl_worldcell_find(name)
	
	if (worldcell_index == -1) then
		return nil
	end
	
	local worldcell = {}
	worldcell.index = worldcell_index
	
	setmetatable(worldcell, tram._metatable_worldcell)
	
	return worldcell
end

-- ============================== AUDIO/AUDIO.H ============================= --

function tram.audio.SetVolume(volume)
	return __impl_audio_set_volume(name)
end

function tram.audio.GetVolume(volume)
	return __impl_audio_get_volume(name)
end

function tram.audio.SetListenerPosition(position)
	return __impl_audio_set_listener_position(position)
end

function tram.audio.SetListenerOrientation(orientation)
	return __impl_audio_set_listener_orientation(orientation)
end





-- ============================ PHYSICS/PHYSICS.H =========================== --

function tram.physics.shape.Sphere(radius)
	local shape = {}
	shape.shape = tram.physics.shape.SPHERE
	shape.radius = radius
	return shape
end

function tram.physics.shape.Cylinder(radius, height)
	local shape = {}
	shape.shape = tram.physics.shape.CYLINDER
	shape.radius = radius
	shape.height = height
	return shape
end

function tram.physics.shape.Capsule(radius, height)
	local shape = {}
	shape.shape = tram.physics.shape.CAPSULE
	shape.radius = radius
	shape.height = height
	return shape
end


function tram.physics.shape.Cone(radius, height)
	local shape = {}
	shape.shape = tram.physics.shape.CONE
	shape.radius = radius
	shape.height = height
	return shape
end


function tram.physics.shape.Box(dimensions)
	local shape = {}
	shape.shape = tram.physics.shape.BOX
	shape.dimensions = dimensions
	return shape
end


function tram.physics.shape.Hull(points)
	local shape = {}
	shape.shape = tram.physics.shape.HULL
	shape.points = points
	return shape
end


function tram.physics.shape.Mesh(triangles)
	local shape = {}
	shape.shape = tram.physics.shape.MESH
	shape.triangles = triangles
	return shape
end

function tram.physics.shape._build(mesh)
	if mesh.shape == tram.physics.shape.SPHERE then
		__impl_physics_collision_set_sphere(shape.radius)
	elseif mesh.shape == tram.physics.shape.CYLINDER then
		__impl_physics_collision_set_cylinder(shape.radius, shape.height)
	elseif mesh.shape == tram.physics.shape.CAPSULE then
		__impl_physics_collision_set_capsule(shape.radius, shape.height)
	elseif mesh.shape == tram.physics.shape.CONE then
		__impl_physics_collision_set_cone(shape.radius, shape.height)
	elseif mesh.shape == tram.physics.shape.BOX then
		__impl_physics_collision_set_box(shape.dimensions)
	elseif mesh.shape == tram.physics.shape.HULL then
		__impl_physics_collision_set_hull()
		for point in mesh.points do
			print("adding point:", point)
			__impl_physics_collision_append_point(point)
		end
	elseif mesh.shape == tram.physics.shape.MESH then
		__impl_physics_collision_set_mesh()
		for triangle in mesh.triangles do
			print("adding triangle:", triangle[1], triangle[2], triangle[3])
			__impl_physics_collision_append_triangle(triangle[1], triangle[2], triangle[3])
		end
	end
end

-- -------------------------------  CONSTANTS ------------------------------- --

tram.physics.shape.SPHERE = 0
tram.physics.shape.CYLINDER = 1
tram.physics.shape.CAPSULE = 2
tram.physics.shape.CONE = 3
tram.physics.shape.BOX = 4
tram.physics.shape.HULL = 5
tram.physics.shape.MESH = 6

-- ========================= PHYSICS/COLLISIONMODEL.H ======================= --

tram.physics._metatable_collisionmodel = {
	__index = {
		GetName = function(self)
			return __impl_physics_collisionmodel_get_name(self.index)
		end
	}
}

function tram.physics.model.Find(name)
	local model_index = __impl_physics_collisionmodel_find(name)
	
	if (model_index == -1) then
		return nil
	end
	
	local model = {}
	model.index = model_index
	
	setmetatable(model, tram.render._metatable_collisionmodel)
	
	return model
end

-- ============================= RENDER/RENDER.H ============================ --

function tram.render.SetSunDirection(direction, layer)
	__impl_render_set_sun_direction(direction, layer)
end

function tram.render.SetSunColor(color, layer)
	__impl_render_set_sun_color(color, layer)
end

function tram.render.SetAmbientColor(color, layer)
	__impl_render_set_ambient_color(color, layer)
end

function tram.render.SetScreenClearColor(color)
	__impl_render_set_screen_clear_color(color)
end

function tram.render.SetViewFov(fov, layer)
	__impl_render_set_view_fov(fov, layer)
end

function tram.render.SetViewDistance(distance, layer)
	__impl_render_set_view_distance(distance, layer)
end

function tram.render.SetViewPosition(direction, layer)
	__impl_render_set_view_position(direction, layer)
end

function tram.render.SetViewRotation(direction, layer)
	__impl_render_set_view_rotation(direction, layer)
end

function tram.render.GetViewPosition(layer)
	return __impl_render_get_view_position(layer)
end

function tram.render.GetViewRotation(layer)
	return __impl_render_get_view_rotation(layer)
end

function tram.render.AddLine(from, to, color)
	__impl_render_add_line(from, to, color)
end

function tram.render.AddLineMarker(position, color)
	__impl_render_add_line_marker(position, color)
end

-- -------------------------------  CONSTANTS ------------------------------- --

tram.render.COLOR_WHITE = tram.math.vec3(1.0, 1.0, 1.0)
tram.render.COLOR_RED = tram.math.vec3(1.0, 0.0, 0.0)
tram.render.COLOR_GREEN = tram.math.vec3(0.0, 1.0, 0.0)
tram.render.COLOR_BLUE = tram.math.vec3(0.0, 0.0, 1.0)
tram.render.COLOR_YELLOW = tram.math.vec3(1.0, 1.0, 0.0)
tram.render.COLOR_PINK = tram.math.vec3(1.0, 0.0, 1.0)
tram.render.COLOR_CYAN = tram.math.vec3(0.0, 1.0, 1.0)
tram.render.COLOR_BLACK = tram.math.vec3(0.0, 0.0, 0.0)
tram.render.COLOR_GRAY = tram.math.vec3(0.3, 0.3, 0.3)

-- =========================== RENDER/ANIMATION.H =========================== --

-- TODO: implement the rest of the stuff

tram.render._metatable_animation = {
	__index = {
		GetName = function(self)
			return __impl_render_animation_get_name(self.index)
		end,
		
		Load = function(self)
			return __impl_render_animation_load(self.index)
		end,
	}
}

function tram.render.animation.Find(name)
	local animation_index = __impl_render_animation_find(name)
	
	if (animation_index == -1) then
		return nil
	end
	
	local animation = {}
	animation.index = animation_index
	
	setmetatable(animation, tram.render._metatable_animation)
	
	return animation
end

-- ============================= RENDER/MODEL.H ============================= --

tram.render._metatable_model = {
	__index = {
		GetName = function(self)
			return __impl_render_model_get_name(self.index)
		end,
		
		AddReference = function(self)
			__impl_render_model_add_reference(self.index)
		end,
		RemoveReference = function(self)
			__impl_render_model_remove_reference(self.index)
		end,
		
		Load = function(self)
			__impl_render_model_load(self.index)
		end,
		
		GetAABBMin = function(self)
			return __impl_render_model_get_aabb_min(self.index)
		end,
		GetAABBMax = function(self)
			return __impl_render_model_get_aabb_max(self.index)
		end,
		
		GetNearDistance = function(self)
			return __impl_render_model_get_near_distance(self.index)
		end,
		GetFarDistance = function(self)
			return __impl_render_model_get_far_distance(self.index)
		end,
		
		SetNearDistance = function(self, dist)
			__impl_render_model_set_near_distance(self.index, dist)
		end,
		SetFarDistance = function(self, dist)
			__impl_render_model_set_far_distance(self.index, dist)
		end
	}
}

function tram.render.model.Find(name)
	local model_index = __impl_render_model_find(name)
	
	if (model_index == -1) then
		return nil
	end
	
	local model = {}
	model.index = model_index
	
	setmetatable(model, tram.render._metatable_model)
	
	return model
end






-- ============================ RENDER/MATERIAL.H =========================== --

-- TODO: implement the rest of the stuff

tram.render._metatable_material = {
	__index = {
		GetName = function(self)
			return __impl_render_material_get_name(self.index)
		end
	}
}

function tram.render.material.Find(name)
	local material_index = __impl_render_material_find(name)
	
	if (material_index == -1) then
		return nil
	end
	
	local material = {}
	material.index = material_index
	
	setmetatable(material, tram.render._metatable_material)
	
	return material
end






-- ============================= RENDER/SPRITE.H ============================ --

-- TODO: implement the rest of the stuff

tram.render._metatable_sprite = {
	__index = {
		GetName = function(self)
			return __impl_render_sprite_get_name(self.index)
		end
	}
}

function tram.render.sprite.Find(name)
	local sprite_index = __impl_render_sprite_find(name)
	
	if (sprite_index == -1) then
		return nil
	end
	
	local sprite = {}
	sprite.index = sprite_index
	
	setmetatable(sprite, tram.render._metatable_sprite)
	
	return sprite
end















-- COMPONENTS COPMPOENNE TSCOMPONENTS CPOMNKPOEN TS COMPONETS

tram.render._metatable_rendercomponent = {
	__index = {
		GetModel = function(self)
			local material = {}
			material.index = __impl_components_render_get_model(self.index)
	
			setmetatable(material, tram.render._metatable_material)
		
			return material
		end,
		
		SetModel = function(self, model)
			if getmetatable(model) ~= tram.render._metatable_model then
				model = tram.render.model.Find(model)
			end
			__impl_components_render_set_model(self.index, model.index)
		end,
		
		SetLightmap = function(self, material)
			if getmetatable(material) ~= tram.render._metatable_material then
				material = tram.render.material.Find(material)
			end
			__impl_components_render_set_lightmap(self.index, material.index)
		end,
		
		SetArmature = function(self, armature)
			assert(getmetatable(armature) == tram.render._metatable_animationcomponent, "SetArmature() accepts only AnimationComponents.")
			__impl_components_set_armature(self.index, armature.index)
			--error("RenderComponent SetArmature not implemented!")
		end,
		
		GetLocation = function(self)
			local vector = __impl_components_render_get_location(self.index)
			setmetatable(vector, tram.math._metatable_vec3)
			return vector
		end,
		
		GetRotation = function(self)
			local vector = __impl_components_render_get_rotation(self.index)
			setmetatable(vector, tram.math._metatable_quat)
			return vector
		end,
		
		SetLocation = function(self, location)
			return __impl_components_render_set_location(self.index, location)
		end,
		
		SetRotation = function(self, rotation)
			return __impl_components_render_set_rotation(self.index, rotation)
		end,
		
		SetScale = function(self, scale)
			return __impl_components_render_set_scale(self.index, scale)
		end,
		
		SetColor = function(self, color)
			return __impl_components_render_set_color(self.index, color)
		end,
		
		SetLayer = function(self, layer)
			return __impl_components_render_set_layer(self.index, layer)
		end,
		
		SetTextureOffset = function(self, material, offset)
			error("RenderComponent SetTextureOffset not implemented!")
		end,
		
		SetDirectionalLight = function(self, enabled)
			return __impl_components_render_set_directional_light(self.index, enabled)
		end,
		
		Init = function(self)
			__impl_components_render_init(self.index)
		end,
		
		Delete = function(self)
			__impl_components_render_delete(self.index)
			table.remove(self, "index")
		end
	}
}

function tram.components.Render()
	local component_index = __impl_components_render_make()
	
	if (component_index == -1) then
		return nil
	end
	
	local component = {}
	component.index = component_index
	
	setmetatable(component, tram.render._metatable_rendercomponent)
	
	return component
end




tram.render._metatable_lightcomponent = {
	__index = {
		SetLocation = function(self, location)
			return __impl_components_light_set_location(self.index, location)
		end,
		
		SetColor = function(self, color)
			return __impl_components_light_set_color(self.index, color)
		end,
		
		SetDistance = function(self, distance)
			return __impl_components_light_set_distance(self.index, distance)
		end,
		
		SetDirection = function(self, direction)
			return __impl_components_light_set_direction(self.index, direction)
		end,
		
		SetExponent = function(self, exponent)
			return __impl_components_light_set_exponent(self.index, exponent)
		end,
		
		Init = function(self)
			__impl_components_light_init(self.index)
		end,
		
		Delete = function(self)
			__impl_components_light_delete(self.index)
			table.remove(self, "index")
		end
	}
}

function tram.components.Light()
	local component_index = __impl_components_light_make()
	
	if (component_index == -1) then
		return nil
	end
	
	local component = {}
	component.index = component_index
	
	setmetatable(component, tram.render._metatable_lightcomponent)
	
	return component
end





tram.render._animationfinishcallbacks = {}

function __impl_components_animation_finish_callback(index, animation)
	local component = {}
	component.index = index
	
	setmetatable(component, tram.render._metatable_animationcomponent)
	
	tram.render._animationfinishcallbacks[index](component, animation)
end

tram.render._metatable_animationcomponent = {
	__index = {
		GetModel = function(self)
			local material = {}
			material.index = __impl_components_animation_get_model(self.index)
	
			setmetatable(material, tram.render._metatable_material)
		
			return material
		end,
		
		SetModel = function(self, model)
			if getmetatable(model) ~= tram.render._metatable_model then
				model = tram.render.model.Find(model)
			end
			__impl_components_animation_set_model(self.index, model.index)
		end,
		
		
		
		SetKeyframe = function(self, name, keyframe)
			__impl_components_animation_set_keyframe(self.index, name, keyframe.location, keyframe.rotation, keyframe.scale)
		end,
		
		SetOnAnimationFinishCallback = function(self, name, callback)
			tram.render._animationfinishcallbacks[self.index] = callback
			__impl_components_animation_add_finish_callback(self.index)
		end,
		
		Play = function(self, name, repeats, weight, speed, interpolate, pause_on_last_frame)
			if repeats == nil then repeats = -1 end
			if weight == nil then weight = 1.0 end
			if speed == nil then speed = 1.0 end
			if interpolate == nil then interpolate = true end
			if pause_on_last_frame == nil then pause_on_last_frame = false end
		
			__impl_components_animation_play(self.index, name, repeats, weight, speed, interpolate, pause_on_last_frame)
		end,
		
		SetKeyframe = function(self, name, keyframe)
			__impl_components_animation_set_keyframe(self.index, name, keyframe.location, keyframe.rotation, keyframe.scale)
		end,
		
		IsPlaying = function(self, name)
			return __impl_components_animation_is_playing(self.index, name)
		end,
		
		Stop = function(self, name)
			__impl_components_animation_stop(self.index, name)
		end,
		
		Pause = function(self, name)
			__impl_components_animation_pause(self.index, name)
		end,
		
		Continue = function(self, name)
			__impl_components_animation_continue(self.index, name)
		end,
		
		
		SetWeight = function(self, name, weight)
			__impl_components_animation_set_weight(self.index, name, weight)
		end,
		
		SetSpeed = function(self, name, speed)
			__impl_components_animation_set_speed(self.index, name, speed)
		end,
		
		SetRepeats = function(self, name, repeats)
			__impl_components_animation_set_repeats(self.index, name, repeats)
		end,
		
		
		FadeIn = function(self, name, length)
			__impl_components_animation_fade_in(self.index, name, length)
		end,
		
		FadeOut = function(self, name, length)
			__impl_components_animation_fade_out(self.index, name, length)
		end,
		
		
		SetPause = function(self, name, pause)
			__impl_components_animation_set_pause(self.index, name, pause)
		end,
		
		SetFade = function(self, name, fade_in, fade_length)
			__impl_components_animation_set_fade(self.index, name, fade_in, fade_length)
		end,
		
		SetFrame = function(self, name, frame)
			__impl_components_animation_set_frame(self.index, name, frame)
		end,
		
		
		Init = function(self)
			__impl_components_animation_init(self.index)
		end,
		
		Delete = function(self)
			__impl_components_animation_delete(self.index)
			table.remove(self, "index")
		end
	}
}

function tram.components.Animation()
	local component_index = __impl_components_animation_make()
	
	if (component_index == -1) then
		return nil
	end
	
	local component = {}
	component.index = component_index
	
	setmetatable(component, tram.render._metatable_animationcomponent)
	
	return component
end


print("\nFinished loading Lua API\n\n")