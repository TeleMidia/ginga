-- $Id: board.lua,v 1.1 2009/03/11 12:09:21 root Exp $

-- board.lua
-- 11mar2009, gflima@telemidia.puc-rio.br
--
-- Holds game board data.

require 'actor_pacman'
require 'actor_ghost'

Board = {

   -- Level file pathname.
   level_file = nil,

   -- Level grid.  Store all static objects (e.g., walls, doors, items, etc.)
   -- Its origin, (1,1), is at top-left corner.
   grid = nil,

   -- Grid size.
   columns = nil,
   lines = nil,

   -- Number of food pellets left in board.
   food = nil,

   -- Ghosts array.
   ghosts = nil,

   -- Our hero.
   pacman = nil,
}

-- Public methods.


--  Creates a new board object and load it with PATHNAME level-file contents.

require 'parser'
function Board:new (pathname)
   assert (pathname, 'Invalid pathname')
   local o = {}
   setmetatable (o, self)
   self.__index = self

   -- Initialize board.
   o.levelfile = pathname
   o.grid = Parser.parse (pathname)
   o.lines = #o.grid
   o.columns = #o.grid[1]

   o.food = 0
   o.pacman = nil
   o.ghosts = {}
   for i = 1, o.lines do
      for j = 1, o.columns do
         local s = o.grid[i][j]

         -- Creates Pacman actor.
         if s == 'pacman' then
            o.pacman = ActorPacman:new (i, j, o)
            o.grid[i][j] = 'empty'

         -- Creates Ghost actor.
         elseif s == 'ghost' then
            o.ghosts[#o.ghosts + 1] = ActorGhost:new (i, j, o)
            o.grid[i][j] = 'empty'

         -- Count food.
         elseif s == 'food' or s == 'freezer' then
            o.food = o.food + 1
         end
      end
   end
   assert (o.pacman, 'Pacman not found')
   return o
end


-- Returns Ghosts array.

function Board:get_ghosts ()
   return self.ghosts
end


-- Returns Pacman.

function Board:get_pacman ()
   return self.pacman
end


-- Returns the coordinates of (I,J)'s next cell in direction DIR.

function Board:next (i, j, dir)
   if dir == 'NORTH' then
      i = ((i - 1) - 1) % self.lines + 1
   elseif dir == 'SOUTH' then
      i = ((i + 1) - 1) % self.lines + 1
   elseif dir == 'WEST' then
      j = ((j - 1) - 1) % self.columns + 1
   elseif dir == 'EAST' then
      j = ((j + 1) - 1) % self.columns + 1
   end
   return i, j
end


-- Reset all actors to their initial configuration.

function Board:reset ()
   self.pacman:reset ()
   for i = 1, #self.ghosts do
      self.ghosts[i]:reset ()
   end
end

-- End: board.lua
