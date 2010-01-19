--[[
-- Support <ref>Draw</ref> objects with texts.
-- Relies on IDirectFBFontProvider for font loading.
-- <br clear="all"/>
-- <example> txt = TextDraw{ text='You win!', font={face='vera.ttf',height=20}, color={r=255,g=0,b=0}, ... }
-- </example>
--]]

local _G = _G
local oo = require 'oo'

module (...)
local gui  = _G.require(_PACKAGE..'gui')
local Draw = _G.require(_PACKAGE..'Draw')

-- Text to be drawn.
text  = oo._REQ

-- Font supported by IDirectFBFontProvider.
font  = oo._REQ

-- Font color in RGB.
color = oo._REQ

-- <hide/>
function new (cls, obj, set)
	local self = _SUPER.new(cls, obj, false)

	-- TEMP: talvez devesse receber DSC completo e nao apenas height
	--[[
	if _G.type(self.font) == 'table' then
		self.font = _load(self.font)
	end
	_G.assert( _G.type(self.font) == 'userdata', 'invalid font' )
	]]--

	self:set(self)
	return self
end

-- <hide/>
function set (self, t)
	if (_G.type(t)=='table') and t.text then
		self.text = t.text
		t.x, t.y = (t.x or {}), (t.y or {})
		_G.canvas:attrFont(self.font.face, self.font.height)
		t.x.d, t.y.d = _G.canvas:measureText(self.text)
	end
	_SUPER.set(self, t)
end

-- <hide/>
function draw (self)
	local parent = self.parent
	_G.canvas:attrFont(self.font.face, self.font.height)
	_G.canvas:attrColor(self.color.r, self.color.g, self.color.b, 255)
	_G.canvas:drawText(self.text, parent.x.p1+self.x.p1, parent.y.p1+self.y.p1);
end

return oo.class(_M, Draw)
