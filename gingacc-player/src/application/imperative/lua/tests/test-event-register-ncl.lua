--[[ test-event-register-ncl.lua -- Check event.register 'ncl' class.
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
-- event.register ([pos:number], f:function, 'ncl', [type:string],
--                 [label or action:string], [action:string])
--     -> filter:table or false, [errmsg:string]
----------------------------------------------------------------------------

-- Invalid type.
assert (bad_argument (event.register (f, 'ncl', {})))

-- Unknown type.
assert (bad_argument (event.register (f, 'ncl', 'x')))

-- Invalid action.
assert (bad_argument (event.register (-32, g, 'ncl', nil, nil, {})))

-- Unknown action.
assert (bad_argument (event.register (-32, g, 'ncl', nil, nil, 'x')))

-- Missing type; label (or name) requires a type.
assert (bad_argument (event.register (h, 'ncl', nil, 'x')))

-- Invalid name (type == 'attribution' requires a name).
assert (bad_argument (event.register (f, 'ncl', 'attribution')))

-- Check sanity.
assert (event.register (f, 'ncl'))                        -- Q=<f>
assert (qsz () == 1)
assert (qf (1) == f and qeq (1, {class='ncl'}))

assert (event.register (g, 'ncl', 'presentation'))        -- Q=<f,g>
assert (qsz () == 2)                                      --      ^
assert (qf (2) == g and qeq (2, {class='ncl', type='presentation'}))

assert (event.register (1, h, 'ncl', 'selection'))        -- Q=<h,f,g>
assert (qsz () == 3)                                      --    ^
assert (qf (1) == h and qeq (1, {class='ncl', type='selection'}))

-- Insert at position 3.
assert (event.register (3, f, 'ncl', nil, nil, 'start'))  -- Q=<h,f,f,g>
assert (qsz () == 4)                                      --        ^
assert (qf (3) == f and qeq (3, {class='ncl', action='start'}))

assert (event.register (g, 'ncl', 'presentation', 'x'))   -- Q=<h,f,f,g,g>
assert (qsz () == 5)                                      --            ^
assert (qf (5) == g)
assert (qeq (5, {class='ncl', type='presentation', label='x'}))

assert (event.register (f, 'ncl', 'presentation', 'y', 'abort'))
assert (qsz () == 6)                                      -- Q=<h,f,f,g,g,f>
assert (qf (6) == f)                                      --              ^
assert (qeq (6, {class='ncl', type='presentation', label='y', action='abort'}))

-- Insert at position 2.
assert (event.register (2, h, 'ncl', 'attribution', 'x')) -- Q=<h,h,f,f,g,g,f>
assert (qsz () == 7)                                      --      ^
assert (qf (2) == h)
assert (qeq (2, {class='ncl', type='attribution', name='x'}))

-- Warning: ignoring extra arguments.
assert (event.register (g, 'ncl', 'attribution', 'y', 'stop', 'z'))
assert (qsz () == 8)                                      -- Q=<h,h,f,f,g,g,f,g>
assert (qf (8) == g)                                      --                  ^
assert (qeq (8, {class='ncl', type='attribution', name='y', action='stop'}))

-- Check whole queue.
assert (qf (1) == h and qeq (1, {class='ncl', type='selection'}))
assert (qf (2) == h and qeq (2, {class='ncl', type='attribution', name='x'}))
assert (qf (3) == f and qeq (3, {class='ncl'}))
assert (qf (4) == f and qeq (4, {class='ncl', action='start'}))
assert (qf (5) == g and qeq (5, {class='ncl', type='presentation'}))
assert (qf (6) == g and qeq (6, {class='ncl', type='presentation', label='x'}))
assert (qf (7) == f and qeq (7, {class='ncl', type='presentation', label='y', action='abort'}))
assert (qf (8) == g and qeq (8, {class='ncl', type='attribution', name='y', action='stop'}))

done ()
