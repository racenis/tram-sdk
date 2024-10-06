
--print(epic_function(420))

--function give_vector(x, y, z)
--	return {x, y, z}
--end


tram.event.Post(tram.event.GetType("my_fancy_event"), 0, nil, nil)

print("VECTOR TSETS!")

vec1 = tram.math.vec3(1, 1, 1)
vec2 = tram.math.vec3(2, 2, 2)
vec3 = vec1 + vec2

print(vec1)
print(vec2)
print(vec3)
print(vec1 + 69)

if toggle == nil then toggle = false end

toggle = not toggle

if toggle then
	tram.message.Send(tram.message.SET_ANIMATION, 0, tram.entity.Find("mongus"):GetID(), "mongus-jump")
else
	tram.message.Send(tram.message.SET_ANIMATION, 0, tram.entity.Find("mongus"):GetID(), "mongus-run")
end

--tram.ui.SetInputState(tram.ui.STATE_MENU_OPEN)

--tram.ui.BindKeyboardKey(tram.ui.KEY_J, tram.ui.KEY_ACTION_JUMP)

--tram.ui.BindKeyboardKey(tram.ui.KEY_J, function() print("chyungine!!") end)


--tram.ui.SetWindowSize(100, 500)
--tram.ui.SetCursor(tram.ui.CURSOR_TEXT)

--print("width:", tram.ui.GetScreenWidth())
--print("height:", tram.ui.GetScreenHeight())