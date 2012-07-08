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
event.register (f, t1)                            -- H=<f>
assert (hsz () == 1 and hf (1) == f and ht (1) == t1)

event.register (g, t2)                            -- H=<f,g>
assert (hsz () == 2)                              --      ^
assert (hf (2) == g and ht (2) == t2)

-- extra argument: ignore.
event.register (1, h, t3, 0)                      -- H=<h,f,g>
assert (hf (1) == h and ht (1) == t3)             --    ^
assert (hf (2) == f and ht (2) == t1)
assert (hf (3) == g and ht (3) == t2)

-- pos < 1: assume pos == 1.
event.register (-5, h, t1)                        -- H=<h,h,f,g>
assert (hsz () == 4)                              --    ^
assert (hf (1) == h and ht (1) == t1)

-- pos == n: insert before the last element.
event.register (hsz (), h, t2)                    -- H=<h,h,f,h,g>
assert (hsz () == 5)                              --          ^
assert (hf (3) == f and ht (3) == t1)
assert (hf (4) == h and ht (4) == t2)
assert (hf (5) == g and ht (5) == t2)

-- pos > n + 1: assume pos == n + 1.
event.register (10, f, t3)                        -- H=<h,h,f,h,g,f>
assert (hsz () == 6)                              --              ^
assert (hf (5) == g and ht (5) == t2)
assert (hf (6) == f and ht (6) == t3)

-- pos == nil: assume pos == n + 1.
event.register (nil, f, t1)                       -- H=<h,h,f,h,g,f,f>
assert (hsz () == 7)                              --                ^
assert (hf (7) == f and ht (7) == t1)

-- filter == nil: assume the empty filter.
event.register (g)
event.register (6, h)                             -- H=<h,h,f,h,g,h,f,f,g>
                                                  --              ^     ^
assert (hsz () == 9)
assert (#ht (9) == 0 and ht (6) == ht (9))

-- Check whole list.
assert (hf (1) == h and ht (1) == t1)
assert (hf (2) == h and ht (2) == t3)
assert (hf (3) == f and ht (3) == t1)
assert (hf (4) == h and ht (4) == t2)
assert (hf (5) == g and ht (5) == t2)
assert (hf (6) == h and #ht (6) == 0)
assert (hf (7) == f and ht (7) == t3)
assert (hf (8) == f and ht (8) == t1)
assert (hf (9) == g and #ht (9) == 0)

-- Success.
done ()
