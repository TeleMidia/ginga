--[[
-- Animates <ref>Draw.lua</ref> objects.
-- The positions are calculated by <ref>func</ref> on each cycle.
-- See <ref>MUVAnim.lua</ref>.
--]]

local _G = _G
local oo = require 'oo'

module (...)
local gui  = _G.require(_PACKAGE..'gui')
local Timer = _G.require(_PACKAGE..'Timer')

-- Draw being animated.
draw = oo._REQ

-- func (params): funcao de calculo por ciclo
func = oo._REQ

-- <hide/>
params = {			-- parametros da funcao de calculo: func() deve setar '_s' para determinado '_t' passado
	x = nil,		--{ _s=nil, _t=nil },
	y = nil,		--{ _s=nil, _t=nil },
}

local _R = {x=nil,y=nil,xp=true}
function onCycle (self)
	for _,p in _G.ipairs(gui._AXIS)
	do
		local t = self.params[p]
		if t then
			--t._s = nil		-- calcula a posicao
			t._t = self.t	-- para o tempo corrente
			self.func(t)
			_R[p] = t._s
		end
	end
	self.draw:set(_R)
end

return oo.class(_M, Timer)
