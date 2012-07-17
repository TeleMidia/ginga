--[[ test-tcp-0.lua -- Check TCP events.
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

-- Sends an HTTP query and dumps the received data.

require 'tests'

local HOSTNAME = 'www.telemidia.puc-rio.br'
local PORT     = 80
local REQUEST  = 'GET / HTTP/1.1\nHost: www.telemidia.puc-rio.br\n\n'

local CID      = nil            -- connection id
local function post(e) return assert (event.post (e)) end

-- Dump the received events.

-- event.register (
--    function (e)
--       print ('--')
--       for k,v in pairs (e) do
--          print (k, v)
--       end
--    end)

-- Open connection.

event.register (
   function (e)
      post ({class='tcp', type='connect', host=HOSTNAME, port=PORT})
   end, {class='ncl', type='presentation', action='start', label=''})

-- Send request.

event.register (
   function (e)
      assert (e.error == nil)
      CID = assert (e.connection)
      post ({class='tcp', type='data', connection=CID, value=REQUEST})
   end, {class='tcp', connection=CID, type='connect'})

-- Dump response data and post the disconnect.

event.register (
   function (e)
      print (assert (e.value))
      post ({class='tcp', connection=CID, type='disconnect'})
   end, {class='tcp', connection=CID, type='data'})

-- Done.

event.register (
   function (e)
      done ()
   end, {class='tcp', connection=CID, type='disconnect'})
