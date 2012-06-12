--[[ test-event-timer.lua -- Check event.timer.
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

-- Invalid calls.
assert (pcall (event.timer) == false)
assert (pcall (event.timer, {}, nil) == false)
assert (pcall (event.timer, 0, {}) == false)

-- Check timer termination after DUR ms.
local DUR = 500

local function handler (e)
   assert (e.dt and teq (e.dt, DUR))
   done ()
end
event.register (handler, 'user')

-- Create timer.
local t0 = event.uptime ()
local function postdt ()
   assert (event.post ('in', {class='user', dt=event.uptime () - t0}))
end
assert (event.timer (DUR, postdt))
