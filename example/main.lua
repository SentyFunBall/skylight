local time = require "cool"

local volume = skylight.getConfigProperty("soundVolume")
print("Sound Volume from config: " .. volume)

local success = skylight.setConfigProperty("soundVolume", volume + 0.1)
if not success then
    print("Failed to set soundVolume")
end

success = skylight.setConfigProperty("wowie", "such config")
if not success then
    print("Failed to set wowie")
end

time.printTime()

function onModLoad()
    print("Wowie we really are here")
end

function onTick(delta)
    print("Tick: " .. delta)
end
