-- $Id: actor.lua,v 1.1 2009/03/11 12:11:13 root Exp $

-- actor.lua
-- 11mar2009, gflima@telemidia.puc-rio.br
--
-- Defines a generic actor interface.

Actor = {

   -- Actor type.
   is_pacman = false,

   -- Direction ('NORTH', 'SOUTH', 'WEST', 'EAST', or 'NONE').
   dir = nil,

   -- Position (i,j).
   start_pos = nil,
   pos = nil,

   -- State.
   state = nil,

   -- Tiles that cannot be transposed.
   barriers = nil,

   -- Game board reference.
   board = nil,
}

-- Public methods.


-- Constructor.

function Actor:new ()
   local o = {}
   setmetatable (o, self)
   self.__index = self
   return o
end


-- Returns an array containing all possible directions.

function Actor:get_possible_dirs ()
   local all = { 'NORTH', 'SOUTH', 'WEST', 'EAST' }
   local dirs = {}
   for d = 1, #all do
      local i, j = self.board:next (self.pos.i, self.pos.j, all[d])
      local tile = self.board.grid[i][j]
      if self:is_barrier (tile) == false then
         dirs[#dirs + 1] = all[d]
      end
   end
   return dirs
end


-- Returns true if actor cannot transpose TILE.

function Actor:is_barrier (tile)
   for i = 1, #self.barriers do
      if self.barriers[i] == tile then
         return true
      end
   end
   return false
end


-- Updates actor configuration.

function Actor:move ()
   self:update_state ()
   self:update_dir ()
   self:update_pos ()
   self:collect ()
   self:collisions ()
end


-- Updates actor position according to its current direction.

function Actor:update_pos ()
   local i, j = self.board:next (self.pos.i, self.pos.j, self.dir)
   if self:is_barrier (self.board.grid[i][j]) then
      return                       -- Nothing to do.
   end
   self.pos.i = i
   self.pos.j = j
end

-- `Abstract' methods.  Specializations must implement these.


-- Collect items.

function Actor:collect ()
   assert (false, 'This method must be implemented')
end


-- Check for collisions.

function Actor:collisions ()
   assert (false, 'This method must be implemented')
end


-- Sets actor state to NAME.

function Actor:set_state (name)
   assert (false, 'This method must be implemented')
end


-- Returns a snapshot (shallow copy) of this actor.

function Actor:snapshot ()
   assert (false, 'This method must be implemented')
end


-- Reset actor to its initial configuration.

function Actor:reset ()
   assert (false, 'This method must be implemented')
end


-- Updates actor direction.

function Actor:update_dir ()
   assert (false, 'This method must be implemented')
end


-- Updates actor current state.

function Actor:update_state ()
   assert (false, 'This method must be implemented')
end

-- End: actor.lua
