print("\n\n\n SCRIPT INIT \n\n\n")

--entity = {}
--tram.entity.Init(entity)

--print("ID:", entity:GetID())
--print("Name:", entity:GetName())


function crate2_c(props)
	props[0] = "kadbusest"

	crate = tram.entity.New("crate", "crate", props)

	crate.OnLoad = function()
		print("\n\n\nCRATE2 LOADING\n\n\n")
	end

	return crate:GetID()
end

function crate2_d()
	print("destructing crate 2")
end

model_prop = {}
model_prop.type = tram.type.NAME
model_prop.flag = 0

collmodel_prop = {}
collmodel_prop.type = tram.type.NAME
collmodel_prop.flag = 0

prop_def = {}
prop_def[0] = model_prop
prop_def[1] = collmodel_prop

tram.entity.Register("crate", prop_def, crate2_c, crate2_d)



props = {}

props.id = 0
props.name = "chungus-crate"
props.flags = 0
props.location = tram.math.vec3(0.0, 10.0, 0.0)
props.rotation = tram.math.quat(tram.math.vec3(0.0, 0.0, 0.0))

props[0] = "crate_ligma"
props[1] = "crate"

crate = tram.entity.New("crate-ext", "crate", props)

crate.OnLoad = function()
	print("Extended immortal crate. Loading...")
end

crate.OnMessageHandler = function(self, message)
	print("Message received:", tram.message.GetName(message.type))
	
	-- do not hand kill messages over to the base class
	if message.type == tram.message.KILL then
		return false
	end
	
	-- allow base class to receive all other messages
	return true
end

print("it has been newed.")
print("my new creat: ", crate:GetName())
print("loding now")
crate:Load()
print("yippee.")





print("registering new event!")
event = tram.event.Register("my_fancy_event")
--event = tram.event.KEYPRESS
print(event)
print(tram.event.GetType("my_fancy_event"))
print(tram.event.GetName(event))

listener = 0

countdown = 3


chamberpot = tram.components.Render()
chamberpot:SetModel("chamberpot2")
chamberpot:Init()

--lamp = tram.components.Light()
--lamp:SetColor(tram.math.vec3(10, 10, 10))
--lamp:SetLocation(tram.math.vec3(0.1, 0.1, 0.1))
--lamp:Init()

function event_function(event)
	print("Hello! this is an event")
	print("Type of this event is ", tram.event.GetName(event.type))
	
	countdown = countdown - 1
	
	if countdown == 0 then
		print("deleting!")
		tram.event.RemoveListener(listener)
	end
end

listener = tram.event.AddListener(event, event_function)


local mark_pos = tram.math.vec3(0, 1, 0)

function tick_function()
	crate = tram.entity.Find("nukta")
	
	if tram.ui.PollKeyboardKey(tram.ui.KEY_J) then
		print("JJJJJ")
		mark_pos = mark_pos + tram.math.vec3(0.05, 0, 0)
	end
	
	if tram.ui.PollKeyboardKey(tram.ui.KEY_K) then
		print("KKKKK")
		mark_pos = mark_pos - tram.math.vec3(0.05, 0, 0)
	end
	
	
	tram.render.AddLineMarker(mark_pos, tram.render.COLOR_WHITE)
	
	
	local pudding = chamberpot:GetLocation()
	pudding.y = 3.25 + 0.5 * math.sin(tram.GetTickTime())
	chamberpot:SetLocation(pudding)
	
	if crate == nil then
		print("O NO WHERE CRATE")
	else
		location = crate:GetLocation()
		location.y = 1.25 + 0.5 * math.sin(tram.GetTickTime())
		crate:SetLocation(location)

		
		--location:poot()
		--print(location)
		
		--rotation = crate:GetRotation()
		--print(rotation)
		--rotation = tram.math.quat(tram.math.vec3(0, tram.GetTickTime(), 0))
		--print(rotation)
		
		local rotation = crate:GetRotation()
		local turn = tram.math.quat(tram.math.vec3(0, 0.01, 0))
		
		--print(turn)
		--print(rotation)
		
		local new_rotation = rotation * turn
		
		--print(new_rotation)
		
		
		crate:SetRotation(new_rotation)
	end
end

tram.event.AddListener(tram.event.TICK, tick_function)
