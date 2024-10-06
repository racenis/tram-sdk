print("\n\n\n SCRIPT INIT \n\n\n")



print("registering new event!")
event = tram.event.Register("my_fancy_event")
--event = tram.event.KEYPRESS
print(event)
print(tram.event.GetType("my_fancy_event"))
print(tram.event.GetName(event))

listener = 0

countdown = 3

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

function tick_function()
	crate = tram.entity.Find("nukta")
	
	if tram.ui.PollKeyboardKey(tram.ui.KEY_J) then
		print("JJJJJ")
	end
	
	if tram.ui.PollKeyboardKey(tram.ui.KEY_K) then
		print("KKKKK")
	end
	
	
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
