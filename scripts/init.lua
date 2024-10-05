print("\n\n\n SCRIPT INIT \n\n\n")



print("registering new event!")
event = tram.event.Register("my_fancy_event")
--event = tram.event.KEYPRESS
print(event)
print(tram.event.GetType("my_fancy_event"))
print(tram.event.GetName(event))


