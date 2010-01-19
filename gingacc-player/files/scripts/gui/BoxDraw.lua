local _G = _G
local oo = require 'oo'

module (...)
local gui  = _G.require(_PACKAGE..'gui')
local Draw = _G.require(_PACKAGE..'Draw')

function collisions (self, d1)
	if not self.visible then return end
	if not d1.visible then return end
	local here = gui.find(self, d1)
	for _, d2 in _G.ipairs(gui.draws)
	do
		if here and (d1 ~= d2) and d2.collide and d2.visible then
			-- TEMP: dois que colidem podem ter dado set() gerando 2 colisoes em vez de apenas uma
			-- TEMP: caso continuem parados e se colidindo, nao pega (teria que postar novo evt collide aqui)
			-- TEMP: passar a intersecao para collide?
			if d1:getIntersection(d2) then
				if _G.type(d1.collide) == 'function' then d1:collide(d2) end
				if _G.type(d2.collide) == 'function' then d2:collide(d1) end
			end
		end
		if (not here) and oo.is(d2,_PACKAGE..'BoxDraw') then
			d2:collisions(d1)
		end
	end
end

-- <hide/>
function draw (self)
	if not self.visible then return end
	for _, draw in _G.ipairs(self) do
		if draw.visible then
			draw:draw()
		end
	end
end

return oo.class(_M, Draw)
