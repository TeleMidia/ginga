-- $Id: input.lua,v 1.1 2009/03/11 11:55:36 root Exp $

-- input.lua
-- 11mar2009, gflima@telemidia.puc-rio.br
--
-- Get input from user.  This is platform-dependent.

Input = {}

local DIR = nil
local KEY_TO_DIR = {
   ['CURSOR_UP'] = 'NORTH',
   ['CURSOR_DOWN'] = 'SOUTH',
   ['CURSOR_LEFT'] = 'WEST',
   ['CURSOR_RIGHT'] = 'EAST',
}


-- Handles key-press events.

local function keypress_handler (evt)
   if evt.class == 'key' and evt.type == 'press' and evt.key then
      DIR = KEY_TO_DIR[evt.key]
   end
end
event.register (keypress_handler)

-- Exported functions.


-- Returns the last selected direction.

local function get_dir ()
   if DIR == nil then
      return 'NONE'
   end
   return DIR
end
Input.get_dir = get_dir


-- Release input handler.

local function unregister ()
   event.unregister (keypress_handler)
end
Input.unregister = unregister

-- End: input.lua
