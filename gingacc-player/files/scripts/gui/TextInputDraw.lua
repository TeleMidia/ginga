local _G       = _G
local table    = table
local string   = string
local oo       = require 'oo'

module (...)
local Timer     = _G.require(_PACKAGE..'Timer')
local TextDraw  = _G.require(_PACKAGE..'TextDraw')
local InputDraw = _G.require(_PACKAGE..'InputDraw')

CHAR_BLINK   =  333
CHAR_TIMEOUT = 1000

charDraw  = nil
charBlink = nil

local write  -- forward declaration

function new (cls, obj, set)
	set = set or (set == nil)
	local self = _SUPER.new(cls, obj, false)
	self.charDraw  = TextDraw{ text='', font=self.font, color=self.color }
	self.charBlink = Timer._blink(self.charDraw, CHAR_BLINK)
	self.charTimeout = Timer{ time=CHAR_TIMEOUT, onEnd=write(self) }
	write(self)() -- forca write() de inicio
	if set then
		self:set()
	end
	return self
end

function set (self, t)
	_SUPER.set(self, t)
	self.charDraw:set{x={p1=self.textDraw.x.p2},y={p1=self.textDraw.y.p1}}
end

function deploy (self, gui)
	_SUPER.deploy(self, gui)
	table.insert(self,       self.charDraw)
	table.insert(gui.timers, self.charTimeout)
	table.insert(gui.timers, self.charBlink)
end

-- TEMP
function remove (gui)
	_G.error'not implemented'
end

local _cur = string.lower
local _def = string.lower
local case = {
	set = function (v)
		if v == true then
			if _cur == string.lower then
				_cur = string.upper
			else
				_cur = string.lower
			end
		elseif v then
			_cur = string[v]
		else
			_cur = _def
		end
		return _cur
	end,
	get = function ()
		return _cur
	end,
}

local ALPHA = {
	  ['1'] = { '1', '.', ',' }
	, ['2'] = { 'a', 'b', 'c', '2' }
	, ['3'] = { 'd', 'e', 'f', '3' }
	, ['4'] = { 'g', 'h', 'i', '4' }
	, ['5'] = { 'j', 'k', 'l', '5' }
	, ['6'] = { 'm', 'n', 'o', '6' }
	, ['7'] = { 'p', 'q', 'r', 's', '7' }
	, ['8'] = { 't', 'u', 'v', '8' }
	, ['9'] = { 'w', 'x', 'y', 'z', '9' }
	, ['0'] = { '0' }
}

-- KEY being pressed and current INDEX in ALPHA
local KEY, IDX = nil, -1

local function showChar (self, show)
	-- "cursor '|'" or "char 'c'" blinking at once
	if show then
		self.charBlink:setStatus('resume')
		self.charDraw:set(true)
		self:set()
		self.cursorBlink:setStatus('pause')
		self.cursorDraw:set(false)
	else
		self.cursorBlink:setStatus('resume')
		self.cursorDraw:set(true)
		self:set()
		self.charBlink:setStatus('pause')
		self.charDraw:set(false)
	end
end

write = function (self)
	return function ()
		KEY, IDX = nil, -1
		self.text = self.text..self.charDraw.text
		self.charDraw.text = ''
		showChar(self, false)
		return 'pause'
	end
end

function handler (self)
	local write = write(self)
	return {
		key = function (evt)
			if evt.type ~= 'press' then return false end
			local key = evt.key

			self.charTimeout:setStatus('restart')

			-- BACKSPACE
			if (key == 'CURSOR_LEFT') then
				if KEY then write() end
				self:set{ text=string.sub(self.text, 1, -2) }

			-- UPPER
			elseif (key == 'CURSOR_UP') then
				if KEY then
					self.charDraw:set{ text=case.set(true)(self.charDraw.text) }
				end

			-- SPACE
			elseif (key == 'CURSOR_RIGHT') then
				if KEY then
					write()
				else
					self:set{ text=self.text..' ' }
				end

			-- NUMBER
			elseif _G.tonumber(key) then
				if KEY ~= key then
					write()
					case.set()
				end
				IDX = (IDX + 1) % #ALPHA[key]
				local v = ALPHA[key][IDX+1]
				self.charDraw:set{ text=case.get()(v) }
				if not KEY then
					showChar(self, true)
				end
				KEY = key
			end
		end
	}
end

return oo.class(_M, InputDraw)
