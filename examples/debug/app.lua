--[[ Copyright (C) 2013-2018 PUC-Rio/Laboratorio TeleMidia

This file is part of NCLua.

NCLua is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

NCLua is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with NCLua.  If not, see <https://www.gnu.org/licenses/>.  ]]--

local canvas = canvas
local event = event
local pairs = pairs
local table = table
local tostring = tostring
local w, h = canvas:attrSize ()
local console =  {}
local count = 0
_ENV = nil

local function dump(tbl)
    local result = {}
    for k, v in pairs(tbl) do
        table.insert(result, k .. "=" .. tostring(v))
    end
    return table.concat(result, ", ")
end

local function event_loop(evt)
    count = count + 1
    table.insert(console, {text=dump(evt), item=count})
    if #console > (h/17) then
        table.remove(console, 1)
    end
end

local function fixed_loop()
    canvas:attrColor (0, 0, 0, 0)
    canvas:clear()
    canvas:attrColor('white')
    canvas:attrFont('sans', 12)
    for i=1, #console do
        local x, y = canvas:measureText(console[i].item)
        canvas:drawText(w - x - 8, 16 * i, console[i].item)
        canvas:drawText(8, 16 * i, console[i].text)
    end
    canvas:flush()
    event.timer(1, fixed_loop)
end

event.register(event_loop)
event.timer(1, fixed_loop)

