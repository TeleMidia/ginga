--[[ test-property-attribution-0.lua -- Check attribution events.
     Copyright (C) 2006-2012 PUC-Rio/Laboratorio TeleMidia

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc., 51
Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. --]]

require 'tests'

local tick      = 0             -- current tick (event serial number)
local start     = nil           -- tick value when start was received
local set_start = nil           -- tick value when set_start was received
local set_stop  = nil           -- tick value when set_stop was received
local a1_start  = nil           -- tick value when a1 start was received
local a2_start  = nil           -- tick value when a2 start was received

-- Uncomment this to enable debug.
event.register (
   function (e)
      print ('--')
      for k,v in pairs (e) do
         print (k,v)
      end
   end)

event.register (
   function (e)
      tick = tick + 1
   end)

event.register (
   function (e)
      start = tick
   end, 'ncl', 'presentation', '', 'start')

event.register (
   function (e)
      set_start = tick
   end, 'ncl', 'attribution', 'P', 'start')

event.register (
   function (e)
      set_stop = tick
   end, 'ncl', 'attribution', 'P', 'stop')

event.register (
   function (e)
      a1_start = tick
   end, 'ncl', 'presentation', 'onBeginAttribution-triggered', 'start')

event.register (
   function (e)
      a2_start = tick
   end, 'ncl', 'presentation', 'onEndAttribution-triggered', 'start')

event.register (
   function (e)
      assert (start == 1)
      assert (set_start == 2)
      assert (set_stop == 3)
      assert (a1_start == 4)
      assert (a2_start == 5)
      assert (tick == 6)
      done ()
   end, 'ncl', 'presentation', '', 'stop')
