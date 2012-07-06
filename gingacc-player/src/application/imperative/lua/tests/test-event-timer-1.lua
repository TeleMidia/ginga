--[[ test-event-timer-1.lua -- Check event.timer cancel.
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

local DUR = 500                 -- test duration in ms

local EOT = false               -- end-of-timer: true if timer's
                                --   function was called;
                                --   that is, the timer was not canceled
function handler(e)
   print ('check called at '..event.uptime ()..'ms')
   assert (not EOT)
   done ()
end
event.register (handler, {class='user'})

local cancel = event.timer (DUR, function () EOT = true end)
assert (cancel)

local function docancel ()
   print ('timer canceled at '..event.uptime ()..'ms')
   cancel ()
end

-- Cancel timer.
assert (event.timer (DUR - TIMEEQ_THRESHOLD, docancel))

-- Call handler.
assert (event.timer (DUR + TIMEEQ_THRESHOLD,
                     function () event.post ('in', {class='user'}) end))
