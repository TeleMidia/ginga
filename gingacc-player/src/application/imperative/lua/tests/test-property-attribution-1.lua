--[[ test-property-attribution-1.lua -- Check attribution events.
     Copyright (C) 2012 PUC-Rio/Laboratorio TeleMidia

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
local a1_start  = nil           -- tick value when a1 start was received

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

      e = {class='ncl', type='attribution', action='start',
           name='P', value='abc'}
      event.post (e)

      e.action = 'stop'
      event.post (e)
   end, {class='ncl', type='presentation', label='', action='start'})

event.register (
   function (e)
      a1_start = tick
   end, {class='ncl', type='presentation',
         label='onBeginAttribution-triggered', action='start'})

event.register (
   function (e)
      assert (start == 1)
      assert (a1_start == 2)
      assert (tick == 3)
      e.label = ''
      e.action = 'stop'
      event.post (e)
      print ('DONE')
      done ()
   end, {class='ncl', type='presentation',
         label='onEndAttribution-triggered', action='start'})
