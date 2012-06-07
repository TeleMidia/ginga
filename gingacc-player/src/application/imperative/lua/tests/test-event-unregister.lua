--[[ test-event-unregister.lua -- Check event.unregister.
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
local h = function () end

local function register5 ()
   assert (event.register (f))
   assert (event.register (g))
   assert (event.register (h))
   assert (event.register (f))
   assert (event.register (g))                    -- Q=<f,g,h,f,g>
   assert (qsz () == 5)
end


----------------------------------------------------------------------------
-- Invalid calls.
----------------------------------------------------------------------------
assert (pcall (event.unregister, 0) == false)
assert (pcall (event.unregister, {}, 0, {}) == false)
assert (pcall (event.unregister, {}, f, f) == false)
assert (pcall (event.unregister, nil, f, f) == false)


----------------------------------------------------------------------------
-- event.unregister (f:function) -> n:number
----------------------------------------------------------------------------

register5 ()                                      -- Q=<f,g,h,f,g>

assert (event.unregister (f) == 2)                -- Q=<g,h,g>
assert (qsz () == 3)
assert (qf (1) == g and qf (2) == h and qf (3) == g)

assert (event.unregister (h) == 1)                -- Q=<g,g>
assert (qsz () == 2)
assert (qf (1) == g and qf (2) == g)

-- Function not registered; do nothing.
assert (event.unregister (f) == 0)
assert (qsz () == 2)
assert (qf (1) == g and qf (2) == g)

assert (event.unregister (g) == 2)                -- Q=<>
assert (qsz () == 0)


----------------------------------------------------------------------------
-- event.unregister (f1:function, f2:function, ...) -> n:number
----------------------------------------------------------------------------

register5 ()                                      -- Q=<f,g,h,f,g>

assert (event.unregister (g, g, g, g) == 2)       -- Q=<f,h,f>
assert (qsz () == 3)
assert (qf (1) == f and qf (2) == h and qf (3) == f)

assert (event.unregister (h, f) == 3)
assert (qsz () == 0)


----------------------------------------------------------------------------
-- event.unregister () -> n:number
----------------------------------------------------------------------------

register5 ()
assert (event.unregister () == 5)
assert (qsz () == 0)

register5 ()
assert (event.unregister (nil) == 5)
assert (qsz () == 0)

assert (event.unregister () == 0)
done ()
