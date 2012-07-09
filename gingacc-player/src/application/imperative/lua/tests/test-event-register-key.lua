--[[ test-event-register-key.lua -- Check event.register for 'key' class.
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

-- Invalid calls.
assert (pcall (event.register, f, 'key', {}) == false)
assert (pcall (event.register, f, 'key', 'press', {}) == false)


----------------------------------------------------------------------------
-- event.register (position:number, function:function, 'key',
--                 [type:string], [key:string])
--   -> true or false, [errmsg:string]
----------------------------------------------------------------------------

assert (event.register (f, 'key', 'x') == false)  -- unknown type

-- Check sanity.
assert (event.register (f, 'key'))                -- Q=<f>
assert (hf (1) == f and heq (1, {class='key'}))

assert (event.register (g, 'key', 'press'))       -- Q=<f,g>
assert (hsz () == 2)                              --      ^
assert (hf (2) == g and heq (2, {class='key', type='press'}))

assert (event.register (h, 'key', 'release'))     -- Q=<f,g,h>
assert (hsz () == 3)                              --        ^
assert (hf (3) == h and heq (3, {class='key', type='release'}))

assert (event.register (2, h, 'key', nil, '1'))   -- Q=<f,h,g,h>
assert (hsz () == 4)                              --      ^
assert (hf (2) == h and heq (2, {class='key', key='1'}))

assert (event.register (f, 'key', 'press', 'RED'))
assert (hsz () == 5)                              -- Q=<f,h,g,h,f>
assert (hf (5) == f)                              --            ^
assert (heq (5, {class='key', type='press', key='RED'}))

-- Extra arguments: ignore.
assert (event.register (g, 'key', 'release', 'GREEN', 0))
assert (hsz () == 6)                              -- Q=<f,h,g,h,f,g>
assert (hf (6) == g)                              --              ^
assert (heq (6, {class='key', type='release', key='GREEN'}))

-- Check whole queue.

assert (hf (1) == f and heq (1, {class='key'}))
assert (hf (2) == h and heq (2, {class='key', key='1'}))
assert (hf (3) == g and heq (3, {class='key', type='press'}))
assert (hf (4) == h and heq (4, {class='key', type='release'}))
assert (hf (5) == f and heq (5, {class='key', type='press', key='RED'}))
assert (hf (6) == g and heq (6, {class='key', type='release', key='GREEN'}))

done ()
