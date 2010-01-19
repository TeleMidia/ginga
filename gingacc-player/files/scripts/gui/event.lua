--[[
-- When the application calls the main <ref>#loop</ref> the application becomes event oriented and all action is taken inside this module.
-- There are four internal events responsible for all the dynamic of an application.
-- They are generated automatically and the programmer should not bother.
-- <ul>
-- <li> 'draw': generated when a <ref>Draw.lua</ref> object changes state (tipically a call to <ref>Draw.lua#set</ref>) and should be redrawn. This event is associated with <ref>gui.lua#draws</ref> collection, as all of its objects will be redrawn. </li>
-- <li> 'collision': whenever there's a possibility of collision this event is generated and the engine checks for collisions. Also associated with <ref>gui.lua#draws</ref> collection. </li>
-- <li> 'timer': while there are active <ref>Timer.lua</ref> objects this event is generated to cycle them (<ref>Timer.lua#onCycle</ref>). This event is associated with <ref>gui.lua#timers</ref> collection. All timers have their elapsed time updated and are automatically removed when ended (<ref>Timer.lua#onEnd</ref>).  </li>
-- <li> 'stop': when this event is generated (<ref>#stop</ref>) the main <ref>#loop</ref> is ended and the control is given back to the main application. </li>
-- </ul>
--
-- The only type of event that needs programmer interaction is for input handling.
-- A handler is a table in which the entries are <code>[event_type] = handler_function</code> pairs, whenever an <code>event_type</code> happens its associated <code>handler_function</code> is called:
-- <example>
-- handler = { [DFEC_INPUT] =
--     function (evt)
--         if     evt.key_symbol == DIKS_CURSOR_UP    then ship:move( 0,-5)
--         elseif evt.key_symbol == DIKS_CURSOR_DOWN  then ship:move( 0, 5)
--         elseif evt.key_symbol == DIKS_CURSOR_LEFT  then ship:move(-5, 0)
--         elseif evt.key_symbol == DIKS_CURSOR_RIGHT then ship:move( 5, 0)
--         elseif evt.key_symbol == DIKS_SPACE        then ship:shoot()
--     end
--  }
--  table.insert(gui.handlers, handler)
--  </example>
--  The above code creates a handler for input events, <code>ship</code> is a <ref>Draw.lua</ref> object and the handler makes it moving on screen.
--]]

local _G        = _G
local coroutine = coroutine
local assert    = assert
local table     = table
local EVENT     = event
local CANVAS    = canvas

module (...)
local gui = _G.require(_PACKAGE..'gui')

-- <hide/>
posted = {}
-- <hide/>
last = nil

-- Main handler for <ref>gui.lua#handlers</ref>, responsible for the dynamics of application after a call to <ref>#loop</ref>.
-- Handles <code>stop, draw, time and collision</code> events.
handler = {}
-- <hide/>
handler =
{
	user = function (evt)
		posted[evt.type] = false
		local f = handler.user_table[evt.type]
		if f then return f(evt) end
	end;

	user_table =
	{
		-- DRAW
		draw = function (evt)
			gui.draws:draw()
			CANVAS:flush()
			return true
		end;

		-- COLLISION
		collision = function (evt)
			gui.draws:collisions(evt.data)
			return true
		end;

		-- TIMER
		timer = function (evt)
			local now = EVENT.uptime()
			local diff = (last and (now - last)) or 0
			last = now
			assert( coroutine.resume(gui.timers.co, gui.timers, diff) )
			return true
		end;
	};
}

-- <hide/>
--local _col = {class='user', type='collision', data=nil}
--local _usr = {class='user', type=nil, data=nil}
-- TEMP: optimize
function post (type, data)
	-- TEMP: deveria ser postado apos draw?
	if (type == 'draw') and data and data.collide and data.visible then
		--_col.data = data
		--EVENT.post('in', _col)
		EVENT.post('in', {class='user', type='collision', data=data})
	end

	if posted[type] then
		return
	end

	if (type=='draw') or (type=='timer') then
		posted[type] = true
	end

	--_usr.type, _usr.data = type, data
	--EVENT.post('in', _usr)
	EVENT.post('in', {class='user', type=type, data=data})
end

-- Exits main <ref>#loop</ref> giving the control back to the application.
-- ret: value returned by main <ref>#loop</ref>
function stop (ret)
	--EVENT.wake(ret)
	EVENT.post('out', { class='ncl', type='presentation', area='', transition='stops' })
end

function listener (evt)
	--_G.collectgarbage'collect'
	-- searches for a single handler for the event
	-- iterates top->down, whenever func returns true, stops
	for i=#gui.handlers, 1, -1 do
		func = gui.handlers[i][evt.class]
		if func then
			if func(evt) then break end  -- handler 'handled' the event, so break
		end
	end
end
EVENT.register(listener)

-- Enters main loop.
-- Every application should call this function after initialization.
function loop ()
	posted.timer, posted.draw = false, false
	post('draw')
	post('timer')
	return EVENT.sleep()
end
