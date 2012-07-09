--[[ test-event-post-ncl.lua -- Check event.post for 'ncl' class.
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
assert (event.post ({class='ncl', type={}}) == false)
assert (event.post ({class='ncl', type='x'}) == false)

-- Invalid or unknown action.
assert (event.post ({class='ncl', type='attribution', action={}}) == false)
assert (event.post ({class='ncl', type='attribution', action='x'}) == false)


-- Attribution:

-- Missing or invalid name.
assert (event.post ({class='ncl', type='attribution', action='start'}) == false)
assert (event.post ({class='ncl', type='attribution', action='start', name={}}) == false)

-- Missing or invalid value.
assert (event.post ({class='ncl', type='attribution', action='start', name='x'}) == false)
assert (event.post ({class='ncl', type='attribution', action='start', name='x', value={}}) == false)

-- Post event and check if it made it to the output queue.
-- TODO: Check output queue.
assert (event.post ({class='ncl', type='attribution', action='start', name='x', value='y'}))
assert (event.post ({class='ncl', type='attribution', action='start', name='x', value='y', x='y', z={}}))


-- Presentation:

-- Missing or invalid label.
assert (event.post ({class='ncl', type='presentation', action='start'}) == false)
assert (event.post ({class='ncl', type='presentation', action='start', label={}}) == false)

-- Post event and check if it made it to the output queue.
-- TODO: Check output queue.
assert (event.post ({class='ncl', type='presentation', action='start', label=''}))
assert (event.post ({class='ncl', type='presentation', action='start', label='', value='y', x='y', z={}}))


-- Selection:

-- Missing or invalid label.
assert (event.post ({class='ncl', type='selection', action='start'}) == false)
assert (event.post ({class='ncl', type='selection', action='start', label={}}) == false)

-- Post event and check if it made it to the output queue.
-- TODO: Check output queue.
assert (event.post ({class='ncl', type='selection', action='start', label=''}))
assert (event.post ({class='ncl', type='selection', action='start', label='', value='y', x='y', z={}}))

done ()
