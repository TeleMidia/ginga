-- $Id: game.lua,v 1.2 2009/03/12 12:02:26 root Exp $

-- game.lua
-- 11mar2009, gflima@telemidia.puc-rio.br
--
-- Game main loop -- `The Glue'.  This is platform-dependent.

require 'board'
require 'display'

-- Interval (in ms) between two game cycles.
local GAME_DELAY = 175

-- Level files pathname.  `%s' will be replaced by the level number.
local LEVEL_PATH = 'levels/%s.txt'


-- Returns true if PATHNAME exists.

local function exists (pathname)
   local f = io.open(pathname)
   if f == nil then
      return false
   end
   io.close(f)
   return true
end


-- Game main loop.

local GAME = nil
do
   GAME = coroutine.create (
      function ()
         local level = 1       -- Initial level.
         local lives = 3       -- Player lives.
         local pathname = LEVEL_PATH:gsub ('%%s', level, 1)

         local win = true

         -- For each level.
         while exists (pathname) and win == true do
            local board = Board:new (pathname)
            local display = Display:new (board)

            while true do

               -- Move actors.
               local ghosts = board:get_ghosts ()
               for i = 1, #ghosts do
                  ghosts[i]:move ()
               end

               local pacman = board:get_pacman ()
               pacman:move ()

               display:redraw (level, lives)
               coroutine.yield ()

               -- Move to next level.
               if board.food <= 0 then
                  level = level + 1
                  pathname = LEVEL_PATH:gsub ('%%s', level, 1)
                  collectgarbage ('collect')
                  break
               end

               -- Decrement lives.
               if board:get_pacman ().state.name == 'DEAD' then
                  lives = lives - 1
                  if lives > 0 then
                     board:reset ()

                  -- Game over; player lose.
                  else
                     win = false
                     break
                  end
               end

            end
         end
         Input.unregister ()
         while true do
            Display.end_message (win)
            coroutine.yield ()
         end
      end
   )
end


-- Event handler.

local TIMER = GAME_DELAY
local function handler (evt)

   -- Start.
   if evt.class == 'ncl' and evt.type == 'presentation'
      and evt.action == 'start' then
      math.randomseed (os.time())
      event.post ('in', { class='user', time=event.uptime ()})
      assert (coroutine.resume (GAME))
   end

   -- Time.
   if evt.class == 'user' and evt.time then
      local now = event.uptime ()
      local dt = now - evt.time

      TIMER = TIMER - dt
      if TIMER <= 0 then
         TIMER = GAME_DELAY

         -- Wake-up game loop.
         local old = event.uptime ()
         assert(coroutine.resume (GAME))
         if coroutine.status (GAME) == 'dead' then
            event.unregister (handler)
            return
         end
         TIMER = TIMER - (event.uptime () - old)
      end
      event.post ('in', { class='user', time=event.uptime () })
   end
end
event.register (handler)

-- End: game.lua
