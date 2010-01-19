--[[
-- Supports <ref>Draw</ref> objects with external images (png, jpg, etc).
-- Relies on IDirectFBImageProvider for image loading.
-- <br clear="all"/>
-- <example>ship = ImageDraw{ image='ship.png', x={ppc=500}, y={ppc=500}, ... }
-- </example>
--]]

local _G = _G
local oo = require 'oo'
local math = math

module (...)
local gui  = _G.require(_PACKAGE..'gui')
local Draw = _G.require(_PACKAGE..'Draw')

-- Image path or canvas object.
image  = oo._REQ

-- Number of frames per line/column on image.
frames = {1,1}

-- Current frame.
frame  = {1,1}

-- Returns an DFBImageProvider for the given path.
-- It caches the images acording to its path name.
local _CACHE = {}
function _load (path)
	if _CACHE[path] then
		return _CACHE[path]
	end
	_CACHE[path] = _G.canvas:new(path)
	return _CACHE[path]
end

-- <hide/>
-- Constructor
-- Has to get image's dimension before calling self:set().
function new (cls, obj, set)
	local self = _SUPER.new(cls, obj, false)
	local path = self.image

	if _G.type(self.image) == 'string' then
		self.image = _load(self.image)
	end
	_G.assert( _G.type(self.image) == 'userdata' )

	local w, h = self.image:attrSize()
	_G.assert( math.mod(w, self.frames[2]) == 0, 'invalid dimensions for image "'.._G.tostring(path)..'"' )
	_G.assert( math.mod(h, self.frames[1]) == 0, 'invalid dimensions for image "'.._G.tostring(path)..'"' )
	self.x.d = w / self.frames[2]
	self.y.d = h / self.frames[1]
	self:set(self)

	return self
end

-- <hide/>
-- Controls frame settings.
function set (self, t)
	_SUPER.set(self, t)
	if t and t.frame then
		self.frame = t.frame
		local dx, dy = self.x.d, self.y.d
	end
end

-- <hide/>
-- Blits current self.frame_surface into parent's surface.
function draw (self)
	local parent = self.parent
	local dx, dy = self.x.d, self.y.d
	_G.canvas:compose(parent.x.p1+self.x.p1, parent.y.p1+self.y.p1, self.image, (self.frame[2]-1)*dx,(self.frame[1]-1)*dy, dx,dy)
end

return oo.class(_M, Draw)
