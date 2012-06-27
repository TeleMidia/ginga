--[[ test-event-register-user.lua -- Check event.register 'user' class.
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

local f = function () end
local g = function () end

-- Check sanity.
assert (event.register (f, 'user'))
for k,v in pairs (queue) do print (k,v) end
assert (qf (1) == f and qeq (1, {class='user'}))

-- Warning: ignoring extra arguments.
assert (event.register (g, 'user', 'x'))
assert (qf (2) == g and qeq (2, {class='user'}))

done ()
