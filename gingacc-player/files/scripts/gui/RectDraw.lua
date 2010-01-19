--[[
-- Supports <ref>Draw.lua</ref> objects with rectangles.
-- Relies on IDirectFBSurface methods to provide rectangles support.
-- <br clear="all"/>
-- <example>rect = RectDraw{ x={p1=0, d=100}, y={p1=0, d=100}, colors={fill={r=0,g=0,b=0}}, ... }
-- </example>
--]]

local _G = _G
local oo = require 'oo'

module (...)
local gui  = _G.require(_PACKAGE..'gui')
local Draw = _G.require(_PACKAGE..'Draw')

-- Table with frame and fill colors.
-- Color is given in { r=[0-255], g=[0-255], b=[0-255] }
colors = {
	frame = nil,
	fill = nil,
}

-- Returns a new full screen Rectangle with the given color.
-- colors: table with frame, fill fields.
function _background (colors)
	colors = colors or _M.colors
	_G.assert(colors.frame or colors.fill)
	return _M{ x={ppc=500,dd=1000}, y={ppc=500,dd=1000}, colors=colors }
end

-- <hide/>
-- Relies on DFBSurface:(Draw/Fill)Rectangle().
function draw (self)
	local parent = self.parent
	local clrs = self.colors
	if clrs.frame then
		_G.canvas:attrColor(clrs.frame.r, clrs.frame.g, clrs.frame.b, clrs.frame.a or 255)
		_G.canvas:drawRect('frame', parent.x.p1+self.x.p1, parent.y.p1+self.y.p1, self.x.d, self.y.d)
	end
	if clrs.fill then
		_G.canvas:attrColor(clrs.fill.r, clrs.fill.g, clrs.fill.b, clrs.fill.a or 255)
		_G.canvas:drawRect('fill', parent.x.p1+self.x.p1, parent.y.p1+self.y.p1, self.x.d, self.y.d)
	end
end

return oo.class(_M, Draw)
