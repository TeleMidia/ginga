--[[ test-event-uptime-1.lua -- Check event.uptime.
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
local start = event.uptime ()

event.register (
   function (e)
      event.post ('in', {class='user', uptime=event.uptime ()})
      return true
   end,
   {class='ncl', type='presentation', action='start', label=''})

event.register (
   function (e)
      assert (e.uptime >= start)
      done ()
   end,
   {class='user'})

event.register (
   function (e)
      fail ()
   end)
