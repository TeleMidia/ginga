--[[ test-event-post-key.lua -- Check event.post for 'key' class.
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

-- Invalid or unknown type.
assert (event.post ({class='key', type={}}) == false)
assert (event.post ({class='key', type='x'}) == false)

-- Invalid or unknown key.
-- TODO: Check if key is known.
assert (event.post ({class='key', type='press', key={}}) == false)

-- Post event and check if it made it to the output queue.
assert (event.post ({class='key', type='press', key='0'}))
assert (event.post ({class='key', type='release', key='1', value='y', x='y', z={}}))

done ()
