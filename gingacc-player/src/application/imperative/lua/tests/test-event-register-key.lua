--[[ test-event-register-key.lua -- Check event.register 'key' class.
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


----------------------------------------------------------------------------
-- event.register (pos:number, f:function, 'key',
--                 [type:string], [key:string])
--     -> filter:table or false, [errmsg:string]
----------------------------------------------------------------------------

-- Invalid type.
assert (bad_argument (event.register (f, 'key', {})))

-- Unknown type.
assert (bad_argument (event.register (f, 'key', 'x')))

-- Invalid key.
assert (bad_argument (event.register (f, 'key', nil, {})))

-- Unknown key.
-- TODO: Check if KEY exists in event.keys array.

-- Check sanity.
assert (event.register (f, 'key'))                -- Q=<f>
assert (qf (1) == f and qeq (1, {class='key'}))

assert (event.register (g, 'key', 'press'))       -- Q=<f,g>
assert (qsz () == 2)                              --      ^
assert (qf (2) == g and qeq (2, {class='key', type='press'}))

assert (event.register (h, 'key', 'release'))     -- Q=<f,g,h>
assert (qsz () == 3)                              --        ^
assert (qf (3) == h and qeq (3, {class='key', type='release'}))

assert (event.register (2, h, 'key', nil, '1'))   -- Q=<f,h,g,h>
assert (qsz () == 4)                              --      ^
assert (qf (2) == h and qeq (2, {class='key', key='1'}))

assert (event.register (f, 'key', 'press', 'RED'))
assert (qsz () == 5)                              -- Q=<f,h,g,h,f>
assert (qf (5) == f)                              --            ^
assert (qeq (5, {class='key', type='press', key='RED'}))

-- Warning: ignoring extra arguments.
assert (event.register (g, 'key', 'release', 'GREEN', 0))
assert (qsz () == 6)                              -- Q=<f,h,g,h,f,g>
assert (qf (6) == g)                              --              ^
assert (qeq (6, {class='key', type='release', key='GREEN'}))

-- Check whole queue.

assert (qf (1) == f and qeq (1, {class='key'}))
assert (qf (2) == h and qeq (2, {class='key', key='1'}))
assert (qf (3) == g and qeq (3, {class='key', type='press'}))
assert (qf (4) == h and qeq (4, {class='key', type='release'}))
assert (qf (5) == f and qeq (5, {class='key', type='press', key='RED'}))
assert (qf (6) == g and qeq (6, {class='key', type='release', key='GREEN'}))

done ()
