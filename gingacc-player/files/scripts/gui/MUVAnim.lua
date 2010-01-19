--TODO:
-- aceitar duas variaveis se uma for o tempo e pegar o valor tempo do outro eixo

--[[
-- Implements MUV (english?) animations for <ref>Draw</ref> objects.
-- <br clear="all"/>
-- <example>
-- local x = { s=nil, s0=self.x.pc, v0=0,    a=0, t=3000 }
-- local y = { s=nil, s0=self.y.pc, v0=-200, a=0, t=3000 }
-- anim = MUVAnim{draw=ship, params={x=x,y=y}}
-- table.insert(gui.timers, anim)
-- </example>
--]]

local _G = _G
local coroutine = coroutine
local oo = require 'oo'

module (...)
local gui  = _G.require(_PACKAGE..'gui')
local Anim = _G.require(_PACKAGE..'Anim')

-- <hide/>
time = false			-- nao mais obriga time=_req (calculado em new)

-- <hide/>
func = function (t) local T = t._t t._s = t._s0 + (T / t._v0) + (T*T / (2 * t._a)) end

-- Params for the formula:
-- s = s0 + v0t + (at^2)/2
-- One of these may be <code>nil</code> and will be calculated.
params = {
	x = nil, --{ s=nil, s0=nil, v0=nil, a=nil, t=nil, },
	y = nil, --{ s=nil, s0=nil, v0=nil, a=nil, t=nil, },
}

-- <hide/>
_MULT = {_v0=1000, _a=1000000}
-- <hide/>
_INF = 1000000000
-- <hide/>
_VARS = {'s','s0','v0','t','a'}

-- CALLBACKS PRONTAS PARA USAR

-- <hide/>
function _backforth (self)
	self.cyclic = true
	for _,axis in ipairs(gui._AXIS)
	do
		local t = self.params[axis]
		if t then
			t._s0 = t._s
			t._v0 = -t._v0
			t._a  = -t._a
		end
	end
end

-- FUNCOES ESTATICAS

-- <hide/>
function _MUV (t)
	-- formula: s = s0 + (v0 * t) + (a*t*t) / 2
	-- 'v', 'v0' e 'a' estao invertidos:
	-- s = s0 + (t / v0) + (t*t / 2*a)
	local _nil = 0
	for _,var in _G.ipairs(_VARS) do
		--print(var, t['_'..var])
		if not t['_'..var] then
			_nil = _nil+1
		end
	end
	if _nil == 0 then return end
	_G.assert(_nil == 1)

	local S, S0, V0, A, T = t._s, t._s0, t._v0, t._a, t._t
	
	local T2 = T and (T * T)
	if not S then
		_G.assert(V0~=0 and A~=0)
		t._s = S0 + (T / V0) + (T2 / (2 * A))
		return
	elseif not S0 then
		_G.assert(V0~=0 and A~=0)
		t._s0 = S - (T / V0) - (T2 / (2 * A))
		return
	elseif not V0 then
		_G.assert(A~=0)
		t._v0 = T / ( (S - S0) - (T2 / (2 * A)) )
		return
	elseif not A then
		_G.assert(V0~=0)
		t._a = T2 / ( 2 * ((S - S0) - (T / V0)) )
		return
	elseif not T then
		_G.assert(A==_INF)
		t._t = ( (V0 == 0) and 0 ) or ( (S - S0) * V0 )
		return
	else
		_G.error()
	end
end

-- <hide/>
function _speedMUV (t)
	return (_MULT._v0 / t._v0) + (t._t * 1000 / t._a)
end

-- <hide/>
function new (cls, obj, start)
	local self = _SUPER.new(cls, obj, false)
	for _,axis in _G.ipairs(gui._AXIS)
	do
		local t = self.params[axis]
		if t
		then
			for _, var in _G.ipairs(_VARS) do
				t['_'..var] = t[var]
			end
			t._s0 = t._s0 or self.draw[axis].pc

			-- variavies invertidas e com mult
			for var, mult in _G.pairs(_MULT) do
				if t[var] then
					t[var] = (t[var] == 0 and _INF) or (mult / t[var])
				end
			end
			_MUV(t)

			if self.time then
				_G.assert( self.time == t._t )
			else
				self.time = t._t
			end
		end
	end
	_G.assert( coroutine.resume(self.co, self, 0) )
	return self
end

return oo.class(_M, Anim)
