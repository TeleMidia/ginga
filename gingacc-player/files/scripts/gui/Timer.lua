--[[
-- This class is the base for all kinds of timers.
-- Timers are implemented using Lua coroutines, although they are hidden in this abstraction and not directly manipulated by the user.
-- The <ref>gui.lua#timers</ref> maintains a collection of Timer objects demanding that any other timer class should derive from this.
-- A timer life is implied by its <ref>time</ref> property and when inserted into above collection the system keeps track of the elapsed time and is responsible for collecting it.
-- The time unit is milliseconds (1000 milliseconds = 1 second).
-- Example:
-- <example>
-- timer = Timer {
--     time = 5000,
--     onEnd = function(self) print '5 seconds have elapsed' end,
-- }
-- table.insert(gui.timers, timer)
-- </example>
--]]

local _G = _G
local coroutine = coroutine
local oo = require 'oo'

module (...)
local event = _G.require(_PACKAGE..'event')

-- Status of <b>self</b>, can be: 'running', 'paused', 'inhurry', 'dead'
status  = nil

-- Ending time for <b>self</b>.
-- If equal to <code>false</code> the timer never ends.
time = oo._REQ

-- If timer should restart when finished.
cyclic = false

-- Function called when the timer ends (reaches its <ref>time</ref> property) with the following signature:
-- <example> function (self) end </example>
onEnd = nil

-- Function called on every cycle for timers with the following signature:
-- <example> function (self) end </example>
onCycle = nil

-- <hide/>
-- associated coroutine
co = nil

-- Elapsed time since its creation (incremented on each cycle).
t = 0

--[[
-- <hide/>
-- Constructor for class.
-- Automatically starts the timer.
-- start: (false) used for classes requiring initialization prior to calling resume(self.co).
-- In this case, <b>self</b> is required to call <code>coroutine.resume(self.co, self, 0)</code> later in its derived constructor.
--]]
function new (cls, obj, start)
	start = start or (start == nil)
	local self = _SUPER.new(cls, obj)
	self.co = coroutine.create(_body)
	self:setStatus('restart')
	if start then
		event.post('timer', self)
	end
	return self
end

function _blink (draw, time)
	return _M{ time=time, cyclic=true, onEnd=function() draw:set(not draw.visible) end }
end

-- Status of <b>self</b>, can be: 'running', 'paused', 'inhurry', 'dead'
local _STATUS = { inhurry=true, paused=true, running=true, restart=true, dead=true }
function setStatus (self, status)
	if status == 'hurry' then
		self.status = 'inhurry'
	elseif status == 'pause' then
		self.status = 'paused'
	elseif status == 'resume' then
		self.status = 'running'
	elseif status == 'restart' then
		self.t = 0;
		self.status = 'running';
	elseif status == 'die' then
		self.status = 'dead'
	else
		_G.error('invalid status '.._G.tostring(status))
	end
end

-- <hide/>
-- Function wrapped in the timer's coroutine.
-- Controls the elapsed time, status and the main loop of the timer.
-- It also calls self:onCycle(), self:onEnd() and is responsible for sending
-- the <ref>event.lua</ref> handler continuation events for itself.
function _body (self, diff)
	-- init
	_G.assert( (self.time==false) or (self.time >= 0) )
	_G.assert( _G.type(self.t) == 'number' )

	-- timer loop
	repeat
		if (self.status == 'inhurry') or (self.status == 'dead') then
			break
		end
		while self.status == 'paused' do
			event.post('timer', self)
			self, diff = coroutine.yield()
		end
		self.t = self.t + diff
		if self.onCycle then
			self:onCycle(diff)
		end
		if (self.time ~= false) and (self.t > self.time) then
			break
		end
		event.post('timer', self)
		self, diff = coroutine.yield()
	until false

	-- someone set it to die
	if self.status == 'dead' then
		return
	end

	-- done
	if self.onEnd then
		-- warning: tail call, last statement
		local status = self:onEnd()
		if status then
			self:setStatus('restart')
			self:setStatus(status)
			return _body(self, 0)
		end
	end

	if self.cyclic then
		-- warning: tail call, last statement
		self:setStatus('restart')
		return _body(self, 0)
	end
end

return oo.class(_M)
