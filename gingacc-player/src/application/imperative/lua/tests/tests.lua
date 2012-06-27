--[[ tests.lua -- Auxiliary functions.
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

_print = print
print = function (...)
   io.stderr:flush ()
   io.stdout:flush ()
   _print (...)
   io.stdout:flush ()
   io.stderr:flush ()
end

-- local _assert = assert
-- function assert (b)
--    return _assert (b) or os.exit (1)
-- end

function bad_argument (status, msg)
   return status == false and msg == "bad argument"
end

function not_implemented (status, msg)
   return status == false and msg == "not implemented"
end

function done ()
   os.exit (0)
end

function fail ()
   os.exit (1)
end

-- We assume the following is working.
queue = assert (event.get_listeners_queue ())
assert (#queue == 0)
function qsz() return #queue end
function qf(i) return queue[i][1] end
function qt(i) return queue[i][2] end

-- Returns true if the contents of qt(i) is the same as t.
function qeq (i, t)
   for k,v in pairs (qt (i)) do
      if t[k] ~= v then return false end
   end
   for k,v in pairs (t) do
      if qt (i)[k] ~= v then return false end
   end
   return true
end

-- Time threshold used in time comparisons.
TEQ_THRESHOLD = 10 -- ms

-- Check if millisecond times t1 and t2 are equal.
function teq (t1, t2)
   return math.abs (t1 - t2) <= TEQ_THRESHOLD
end
