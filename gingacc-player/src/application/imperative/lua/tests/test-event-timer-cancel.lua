--[[ test-event-timer-cancel.lua -- Check event.timer cancel.
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

local DUR = 500                 -- test duration in ms

local EOT = false               -- end-of-timer: true if timer's
                                --   function was called;
                                --   that is, the timer was not canceled

local THRESHOLD = TEQ_THRESHOLD

function handler(e)
   assert (not EOT)
   done ()
end
event.register (handler, 'user')

local cancel = event.timer (DUR, function () EOT = true end)
assert (cancel)

-- Cancel timer.
assert (event.timer (DUR - THRESHOLD - 1, cancel))

-- Call handler.
assert (event.timer (DUR + THRESHOLD + 1, function () event.post ('in', {class='user'}) end))
