--
-- TODO
-- fontes, cores e outros parametros, de onde vem?
-- alpha ignorado
-- Reset evts
-- no prompt: SIM e NAO?
--

local _G = _G
local table = table
local math  = math

local directfb  = require 'directfb'
local gui       = require 'gui.gui'
local event     = require 'gui.event'
local TextDraw  = require 'gui.TextDraw'
local RectDraw  = require 'gui.RectDraw'
local Timer     = require 'gui.Timer'

module (...)

-- F1 = RED
-- F2 = GREEN
-- F3 = YELLOW
-- F4 = BLUE

COLORS = {
	foreground = { r=0x00, g=0x00, b=0x00 };
	background = { r=0xff, g=0xff, b=0xff };
}

TextDraw.font = { face='vera.ttf', height=20 }
local InputDraw = _G.require 'gui.InputDraw'

PADDING = 2

local _draws, _timers, _handlers
function save ()
	_draws    = #gui.draws    + 1
	_timers   = #gui.timers   + 1
	_handlers = #gui.handlers + 1
end

function restore ()
	for i=_draws,    #gui.draws    do gui.draws[i]    = nil end
	for i=_timers,   #gui.timers   do gui.timers[i]   = nil end
	for i=_handlers, #gui.handlers do gui.handlers[i] = nil end
end

function alert (message, showButton, left, top, width, height, alpha, timeout)
	save()

	COLORS.background.a = _G.assert(_G.tonumber(alpha))
	COLORS.foreground.a = _G.assert(_G.tonumber(alpha))

	local rect = RectDraw._background{fill=COLORS.background}
	table.insert(gui.draws, rect)
	message = TextDraw{ text=message, parent=rect, color=COLORS.foreground }
	table.insert(gui.draws, message)

	if width == -1 then
		width = message.x.d + 2*PADDING
	end
	if height == -1 then
		height = message.y.d + 2*PADDING
	end
	if showButton then
		showButton = TextDraw{ text="OK", parent=rect, color=COLORS.foreground }
		height = height + showButton.y.d + PADDING
		table.insert(gui.draws, showButton)
	end

	rect:set{ x={d=width,ppc=500}, y={d=height,ppc=500} }
	message:set{ x={p1=left+PADDING}, y={p1=top+PADDING} }
	if showButton then
		showButton:set{ x={ppc=500}, y={p1=message.y.p2+PADDING} }
	end

	table.insert(gui.timers, Timer{time=(timeout or false), onEnd=function() event.stop() end})
	table.insert(gui.handlers, { [directfb.DFBEventClass.DFEC_INPUT] =
		function (evt)
			if evt.type ~= directfb.DFBInputEventType.DIET_KEYPRESS then return end
			if evt.key_symbol == directfb.DFBInputDeviceKeySymbol.DIKS_F1 then
				event.stop()
			end
		end
	})

	event.loop()
	restore()
end

function confirm (message, left, top, width, height, alpha, timeout)
	save()

	COLORS.background.a = _G.assert(_G.tonumber(alpha))
	COLORS.foreground.a = _G.assert(_G.tonumber(alpha))
	local yesno = "F1-Nao     F2-Sim"

	local rect = RectDraw._background{fill=COLORS.background}
	table.insert(gui.draws, rect)

	message = TextDraw{ text=message, parent=rect, color=COLORS.foreground }
	table.insert(gui.draws, message)
	yesno   = TextDraw{ text=yesno,   parent=rect, color=COLORS.foreground }
	table.insert(gui.draws, yesno)

	if width == -1 then
		width = math.max(message.x.d, yesno.x.d) + 2*PADDING
	end
	if height == -1 then
		height = message.y.d + yesno.y.d + 3*PADDING
	end
	rect:set{ x={d=width,ppc=500}, y={d=height,ppc=500} }

	message:set{ x={p1=left+PADDING}, y={p1=top+PADDING} }
	yesno:set{ x={ppc=500}, y={p1=message.y.p2+PADDING} }

	table.insert(gui.timers, Timer{time=(timeout or false), onEnd=function() event.stop(false) end})
	table.insert(gui.handlers, { [directfb.DFBEventClass.DFEC_INPUT] =
		function (evt)
			if evt.type ~= directfb.DFBInputEventType.DIET_KEYPRESS then return end
			if evt.key_symbol == directfb.DFBInputDeviceKeySymbol.DIKS_F1 then
				event.stop(false)
			elseif evt.key_symbol == directfb.DFBInputDeviceKeySymbol.DIKS_F2 then
				event.stop(true)
			end
		end
	})

	local ret = event.loop()
	restore()
	return ret
end

function prompt (message, defaultValue, left, top, width, height, alpha, timeout)
	save()

	COLORS.background.a = _G.assert(_G.tonumber(alpha))
	COLORS.foreground.a = _G.assert(_G.tonumber(alpha))
	local yesno = "F1-OK"

	local rect = RectDraw._background{fill=COLORS.background}
	table.insert(gui.draws, rect)

	message = TextDraw{ text=message, parent=rect, color=COLORS.foreground }
	table.insert(gui.draws, message)
	input   = InputDraw{ text=defaultValue, parent=rect, color=COLORS.foreground }
	table.insert(gui.draws, input)
	table.insert(gui.handlers, input)
	yesno   = TextDraw{ text=yesno,   parent=rect, color=COLORS.foreground }
	table.insert(gui.draws, yesno)

	if width == -1 then
		width = math.max(message.x.d, input.x.d, yesno.x.d) + 2*PADDING
	end
	if height == -1 then
		height = message.y.d + input.y.d + yesno.y.d + 4*PADDING
	end
	rect:set{ x={d=width,ppc=500}, y={d=height,ppc=500} }

	message:set{ x={p1=left+PADDING}, y={p1=top+PADDING} }
	input  :set{ x={p1=message.x.p1}, y={p1=message.y.p2+PADDING} }
	yesno  :set{ x={ppc=500},         y={p1=input.y.p2+PADDING} }

	table.insert(gui.timers, Timer{time=(timeout or false), onEnd=function() event.stop(false) end})
	table.insert(gui.handlers, { [directfb.DFBEventClass.DFEC_INPUT] =
		function (evt)
			if evt.type ~= directfb.DFBInputEventType.DIET_KEYPRESS then return end
			if evt.key_symbol == directfb.DFBInputDeviceKeySymbol.DIKS_F1 then
				event.stop(input.text)
			elseif evt.key_symbol == directfb.DFBInputDeviceKeySymbol.DIKS_F2 then
				event.stop(input.text)
			else
				local num = evt.key_symbol - directfb.DFBInputDeviceKeySymbol.DIKS_0
				if num >= 0 and num < 10 then
					input:set{ text=input.text..num }
				end
			end
		end
	})

	local ret = event.loop()
	restore()
	return (ret ~= '') and ret
end

--[[
--function alert (message, showButton, left, top, width, height, alpha, timeout)
alert('oioioioioi', true, -1, -1, -1, -1, 255, 10000)
event.buffer:Reset()
local ret = confirm('Confirma a selecao?', 0, 0, -1, -1, 0x00, 5)
_G.print("CONFIRM:", ret)
ret = prompt('Entre com seu peso:', 'peso', 0, 0, -1, -1, 0x00, 5)
_G.print("PROMPT:", ret)
]]--
