local _G = _G
local oo = require 'oo'
local string = string
local table = table

-- TODO:
-- documentar que o texto cresce de acordo com o alinhamento
-- T9?

module (...)
local BoxDraw  = _G.require(_PACKAGE..'BoxDraw')
local TextDraw = _G.require(_PACKAGE..'TextDraw')
local Timer    = _G.require(_PACKAGE..'Timer')

-- Initial input.
text  = ''
font  = nil
color = nil

CURSOR_BLINK = 800

textDraw    = nil
cursorDraw  = nil
cursorBlink = nil

function new (cls, obj, set)
	set = set or (set == nil)
	local self = _SUPER.new(cls, obj, false)
	self.textDraw    = TextDraw{ parent=self, text=self.text, font=self.font, color=self.color }
	self.cursorDraw  = TextDraw{ parent=self, text='|',       font=self.font, color=self.color }
	self.cursorBlink = Timer._blink(self.cursorDraw, CURSOR_BLINK)
	table.insert(self, self.textDraw)
	table.insert(self, self.cursorDraw)
	if set then
		self:set()
	end
	return self
end

function set (self, t)
	_SUPER.set(self, t)
	local text = ((_G.type(t)=='table') and t.text) or self.text
	self.text = text
	self.textDraw:set{text=text, x={p1=0},y={p1=0}}
	self.cursorDraw:set{x={p1=self.textDraw.x.p2},y={p1=self.textDraw.y.p1}}
end

function deploy (self, gui)
	self._handler = self:handler()
	table.insert(gui.draws,    self)
	table.insert(gui.timers,   self.cursorBlink)
	table.insert(gui.handlers, self._handler)
end

-- TEMP
function undeploy (self, gui)
	gui.remove(gui.draws,    self)
	gui.remove(gui.timers,   self.cursorBlink)
	gui.remove(gui.handlers, self._handler)
end

function clear (self)
	self:set{ text='' }
end

function handler (self)
	return {
		key = function (evt)
			if evt.type ~= 'press' then return end
			local key = evt.key
			local text = self.text

			-- BACKSPACE
			if (key == 'CURSOR_LEFT') then
				text = string.sub(text, 1, -2)

			-- SPACE
			elseif (key == 'CURSOR_RIGHT') then
				text = text .. ' '

			-- NUMBER
			elseif _G.tonumber(key) then
				text = text..key
			end

			self:set{ text=text }
		end
	}
end

return oo.class(_M, BoxDraw)
