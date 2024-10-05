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

