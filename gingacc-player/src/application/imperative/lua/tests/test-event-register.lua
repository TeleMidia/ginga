--[[ test-event-register.lua -- Check event.register.
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

local f = function () end
local g = function () end
local h = function () end

local t1 = {1}
local t2 = {1,2}
local t3 = {1,2,3}

-- Invalid calls.
assert (pcall (event.register) == false)
assert (pcall (event.register, 0) == false)
assert (pcall (event.register, {}, {}) == false)
assert (pcall (event.register, {}, f) == false)
assert (pcall (event.register, '', f) == false)


----------------------------------------------------------------------------
-- event.register ([position:number], function:function, [filter:table])
----------------------------------------------------------------------------

-- Check sanity.
event.register (f, t1)                            -- Q=<f>
assert (qsz () == 1 and qf (1) == f and qt (1) == t1)

event.register (g, t2)                            -- Q=<f,g>
assert (qsz () == 2)                              --      ^
assert (qf (2) == g and qt (2) == t2)

-- extra argument: ignore.
event.register (1, h, t3, 0)                      -- Q=<h,f,g>
assert (qf (1) == h and qt (1) == t3)             --    ^
assert (qf (2) == f and qt (2) == t1)
assert (qf (3) == g and qt (3) == t2)

-- pos < 1: assume pos == 1.
event.register (-5, h, t1)                        -- Q=<h,h,f,g>
assert (qsz () == 4)                              --    ^
assert (qf (1) == h and qt (1) == t1)

-- pos == n: insert before the last element.
event.register (qsz (), h, t2)                    -- Q=<h,h,f,h,g>
assert (qsz () == 5)                              --          ^
assert (qf (3) == f and qt (3) == t1)
assert (qf (4) == h and qt (4) == t2)
assert (qf (5) == g and qt (5) == t2)

-- pos > n + 1: assume pos == n + 1.
event.register (10, f, t3)                        -- Q=<h,h,f,h,g,f>
assert (qsz () == 6)                              --              ^
assert (qf (5) == g and qt (5) == t2)
assert (qf (6) == f and qt (6) == t3)

-- pos == nil: assume pos == n + 1.
event.register (nil, f, t1)                       -- Q=<h,h,f,h,g,f,f>
assert (qsz () == 7)                              --                ^
assert (qf (7) == f and qt (7) == t1)

-- filter == nil: assume the empty filter.
event.register (g)
event.register (6, h)                             -- Q=<h,h,f,h,g,h,f,f,g>
                                                  --              ^     ^
assert (qsz () == 9)
assert (#qt (9) == 0 and qt (6) == qt (9))

-- Check whole queue.
assert (qf (1) == h and qt (1) == t1)
assert (qf (2) == h and qt (2) == t3)
assert (qf (3) == f and qt (3) == t1)
assert (qf (4) == h and qt (4) == t2)
assert (qf (5) == g and qt (5) == t2)
assert (qf (6) == h and #qt (6) == 0)
assert (qf (7) == f and qt (7) == t3)
assert (qf (8) == f and qt (8) == t1)
assert (qf (9) == g and #qt (9) == 0)

-- Success.
done ()
