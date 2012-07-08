--[[ test-event-register-ncl.lua -- Check event.register 'ncl' class.
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
assert (pcall (event.register, f, 'ncl', {}) == false)
assert (pcall (event.register, f, 'ncl', 'presentation', f) == false)
assert (pcall (event.register, f, 'ncl', 'presentation', 'x', {}) == false)


----------------------------------------------------------------------------
-- event.register ([position:number], function:function, 'ncl',
--                 [type:string, [label:string]], [action:string])
--   -> true or false, [errmsg:string]
----------------------------------------------------------------------------

assert (event.register (f, 'ncl', 'x') == false)           -- unknown type
assert (event.register (f, 'ncl', nil, nil, 'x') == false) -- unknown action

-- Check sanity.
assert (event.register (f, 'ncl'))                        -- H=<f>
assert (hsz () == 1)
assert (hf (1) == f and heq (1, {class='ncl'}))

assert (event.register (g, 'ncl', 'presentation'))        -- H=<f,g>
assert (hsz () == 2)                                      --      ^
assert (hf (2) == g and heq (2, {class='ncl', type='presentation'}))

assert (event.register (1, h, 'ncl', 'selection'))        -- H=<h,f,g>
assert (hsz () == 3)                                      --    ^
assert (hf (1) == h and heq (1, {class='ncl', type='selection'}))

-- Insert at position 3.
assert (event.register (3, f, 'ncl', nil, nil, 'start'))  -- H=<h,f,f,g>
assert (hsz () == 4)                                      --        ^
assert (hf (3) == f and heq (3, {class='ncl', action='start'}))

assert (event.register (g, 'ncl', 'presentation', 'x'))   -- H=<h,f,f,g,g>
assert (hsz () == 5)                                      --            ^
assert (hf (5) == g)
assert (heq (5, {class='ncl', type='presentation', label='x'}))

assert (event.register (f, 'ncl', 'presentation', 'y', 'abort'))
assert (hsz () == 6)                                      -- H=<h,f,f,g,g,f>
assert (hf (6) == f)                                      --              ^
assert (heq (6, {class='ncl', type='presentation', label='y', action='abort'}))

-- Insert at position 2.
assert (event.register (2, h, 'ncl', 'attribution', 'x')) -- H=<h,h,f,f,g,g,f>
assert (hsz () == 7)                                      --      ^
assert (hf (2) == h)
assert (heq (2, {class='ncl', type='attribution', name='x'}))

-- Extra arguments: ignore.
assert (event.register (g, 'ncl', 'attribution', 'y', 'stop', 'z'))
assert (hsz () == 8)                                      -- H=<h,h,f,f,g,g,f,g>
assert (hf (8) == g)                                      --                  ^
assert (heq (8, {class='ncl', type='attribution', name='y', action='stop'}))

-- Check whole list.
assert (hf (1) == h and heq (1, {class='ncl', type='selection'}))
assert (hf (2) == h and heq (2, {class='ncl', type='attribution', name='x'}))
assert (hf (3) == f and heq (3, {class='ncl'}))
assert (hf (4) == f and heq (4, {class='ncl', action='start'}))
assert (hf (5) == g and heq (5, {class='ncl', type='presentation'}))
assert (hf (6) == g and heq (6, {class='ncl', type='presentation', label='x'}))
assert (hf (7) == f and heq (7, {class='ncl', type='presentation', label='y', action='abort'}))
assert (hf (8) == g and heq (8, {class='ncl', type='attribution', name='y', action='stop'}))

done ()
