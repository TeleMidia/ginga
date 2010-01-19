local _G = _G
local oo = require 'oo'
local coroutine = coroutine

module (...)
local Timer = _G.require(_PACKAGE..'Timer')

time = false  -- until all childs are done
auto = true

function onCycle (self, diff)
	local dead = true
	for i, timer in _G.pairs(self) do
		if _G.type(i) == 'number' then
			if coroutine.status(timer.co) == 'dead' then
				dead = false
				self[i] = nil
			else
				_G.assert( coroutine.resume(timer.co, timer, diff) )
			end
		end
	end
	if self.auto and dead then
		self:setStatus('die')
	end
	return true
end

return oo.class(_M, Timer)
