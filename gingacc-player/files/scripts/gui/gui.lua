local _G = _G
local table = table

module (...)
local event    = _G.require(_PACKAGE..'event')
local BoxDraw  = _G.require(_PACKAGE..'BoxDraw')
local BoxTimer = _G.require(_PACKAGE..'BoxTimer')

-- <hide/>
-- The 2D axis.
_AXIS = {'x','y'}

-- <hide/>
-- Encapsulates the top window into a fake <ref>Draw.lua</ref> object.
local w, h = _G.canvas:attrSize()
parent = { x={p1=0,d=w}, y={p1=0,d=h}, class=true }

--[[
-- Collection of <ref>Draw.lua</ref> objects.
-- Every created <ref>Draw.lua</ref> object must be inserted into this collection to have:
-- <ul> <li> Automatic redrawing when needed. </li> <li> Z-order respecting order of insertion. </li> <li> A basic collision detection system (see also <ref>Draw.lua#collide</ref>). </li> </ul>
--]]
draws = BoxDraw{ x={p1=0, dd=1000}, y={p1=0, dd=1000} }

--[[
-- Collection of <ref>Timer.lua</ref> objects.
-- As for the <ref>draws</ref> collection, every <ref>Timer.lua</ref> object created must be inserted into this to have:
-- <ul> <li> Automatic execution (<ref>Timer.lua</ref> objects are lua coroutines). </li>  <li> Detection and removal on end. </li> </ul>
--]]
timers = BoxTimer{ auto=false }

--[[
-- Collection of <ref>Handler.lua</ref> objects.
-- Keeps the list of active applications handlers.
-- <ref>event.lua#loop</ref> will iterate over this collection calling the appropriate handler when a matching event happens.
-- Some internal handlers to the engine are automatically put into this collection.
-- Other handlers like <i>input events handlers</i> are application specific and should be inserted explicitily.
-- See <ref>event.lua</ref> for the specification of a handler.
--]]
handlers = { event.handler }

function remove (list, obj)
	for i, v in _G.ipairs(list) do
		if v == obj then
			table.remove(list, i)
			return true
		end
	end
	return false
end

function find (list, obj)
	for i, v in _G.ipairs(list) do
		if v == obj then
			return i
		end
	end
	return nil
end

--[[
-- CODE
-- Creates an FPS draw/anim showing the current FPS rate in the screen.
--]]

--local TextDraw = _G.require 'gui.TextDraw'
--local Timer    = _G.require 'gui.Timer'

-- the draw object is created and inserted into the draws collection
--table.insert(draws, TextDraw{ color={r=0,g=0,b=0}, font={face='vera.ttf',height=10}, text='???', x={pp2=1000}, y={pp2=1000} })

-- the anim object is created and inserted into the timers collection
-- every cycle() call increment its self.cycles counter and after 1000ms the
-- animation is done, calling the callback() which sets the FPS text
-- the animation is cyclic, meaning that when done it is automatically
-- restarted
--[[
table.insert(timers, Timer {
	  time = 1000
	, draw = draws[#draws]
	, cycles = 0
	, onCycle = function (self) self.cycles = self.cycles + 1 end
	, onEnd   = function(self)
		local times = self.times
		self.draw:set{ text =
			self.cycles..' FPS | '..
			#draws..' draws | '..
			#timers..' timers | '..
			_G.collectgarbage'count'..' used mem'
		}
		self.cycles = 0
	end
	, cyclic = true
})
]]--

-- TEMP: falar de OO
-- TEMP: incluir um exemplo completo e besta

--[[
-- <project>Lua GUI Engine</project>
-- The Lua GUI Engine is a collection of Lua classes to handle the underlying graphics system with a high-level interface (in this case the DirectFB graphics system).
-- It provides an API to build applications making extensible use of graphic interfaces.
-- This module maintains a collection of <ref>#draws</ref>, <ref>#timers</ref> and <ref>#handlers</ref>, the three main abstractions used by the other modules in the engine.
-- This engine is event oriented meaning that after initialization the engine enters in a main loop (<ref>event.lua#loop</ref>).
--
-- <section><title>Lua Files</title> <ul><li> <ref>gui.lua</ref> </li> <li> <ref>event.lua</ref> </li> <li> <ref>Draw.lua</ref> </li> <li> <ref>RectDraw.lua</ref> </li> <li> <ref>ImageDraw.lua</ref> </li> <li> <ref>TextDraw.lua</ref> </li> <li> <ref>Timer.lua</ref> </li> <li> <ref>Anim.lua</ref> </li> <li> <ref>MUVAnim.lua</ref> </li> </ul>
-- </section>
--
-- <section><title>Architecture</title>
-- <img src="arch.png"/>
-- </section>
--]]
