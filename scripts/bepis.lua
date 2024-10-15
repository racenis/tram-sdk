
--print(epic_function(420))

--function give_vector(x, y, z)
--	return {x, y, z}
--end

--tram.render.SetScreenClearColor(tram.render.COLOR_WHITE)

--event = {}
--event.type = tram.event.GetType("my_fancy_event")
--event.subtype = 0
--event.poster = nil
--event.data = nil

--tram.event.Post(event)

--print("VECTOR TSETS!")

--vec1 = tram.math.vec3(1, 1, 1)
--vec2 = tram.math.vec3(2, 2, 2)
--vec3 = vec1 + vec2

--print(vec1)
--print(vec2)
--print(vec3)
--print(vec1 + 69)

--if toggle == nil then toggle = false end

--toggle = not toggle

--msg = {}
--msg.type = tram.message.SET_ANIMATION
--msg.sender = 0
--msg.receiver = tram.entity.Find("mongus"):GetID()

--if toggle then
--	msg.data = "mongus-jump"
--	tram.message.Send(msg)
--else
--	msg.data = "mongus-run"
--	tram.message.Send(msg)
--end

--mongus = tram.render.animation.Find("mongus-run")

--print("mongus: ", mongus)
--print("name: ", mongus:GetName())



model = tram.components.Render()
model:SetModel("mongus")
model:Init()

player = tram.components.Animation()
player:SetModel("mongus")
player:Init()

model:SetArmature(player)

print("is playing bef ", player:IsPlaying("mongus-run"))
player:Play("mongus-run", 100)
print("is playing now ", player:IsPlaying("mongus-run"))

--tram.ui.SetInputState(tram.ui.STATE_MENU_OPEN)

--tram.ui.BindKeyboardKey(tram.ui.KEY_J, tram.ui.KEY_ACTION_JUMP)

--tram.ui.BindKeyboardKey(tram.ui.KEY_J, function() print("chyungine!!") end)


--tram.ui.SetWindowSize(100, 500)
--tram.ui.SetCursor(tram.ui.CURSOR_TEXT)

--print("width:", tram.ui.GetScreenWidth())
--print("height:", tram.ui.GetScreenHeight())