-- $Id: actor_ghost.lua,v 1.1 2009/03/11 12:00:54 root Exp $

-- actor_ghost.lua
-- 11mar2009, gflima@telemidia.puc-rio.br
--
-- Holds Ghost data and control logic.

require 'actor'

ActorGhost = Actor:new ()

local GHOST_STATES = {

   -- Ghost is waiting to be released.  (During this state he cannot transpose
   -- doors.)
   ['WAITING'] = {
      name = 'WAITING',
      timer = 20,           -- When reaches 0 indicates a state change.
   },

   -- Ghost is fine; chase Pacman.
   ['OK'] = { name = 'OK' },

   -- Ghost is frozen; run-away from Pacman.
   ['FROZEN'] = {
      name = 'FROZEN',
      timer = 40,
   },

   -- Ghost was killed and is waiting to appear again in its initial position.
   ['DEAD'] = {
      name = 'DEAD',
      timer = 20,
   },
}

-- Actor interface implementation.


function ActorGhost:new (i, j, board)
   assert (i > 0 and j > 0 and board, 'Invalid constructor parameters')
   local o = {}
   setmetatable (o, self)
   self.__index = self

   -- Initialize ghost.
   o.is_pacman = false
   o.dir = 'NONE'
   o.start_pos = { i=i, j=j }
   o.pos = { i=i, j=j }
   o.barriers = {'wall'}
   o.board = board
   o.state = {}; o:set_state ('WAITING')
   return o
end


function ActorGhost:collect ()
   -- Nothing to do.
end


-- Check for collisions with Pacman.

function ActorGhost:collisions ()
   if self.state.name == 'DEAD' then return end

   local pacman = self.board:get_pacman ()

   -- Is colliding.
   if self.pos.i == pacman.pos.i and self.pos.j == pacman.pos.j then

      -- Kill self.
      if self.state.name == 'FROZEN' then
         self:set_state ('DEAD')

      -- Kill Pacman.
      else
         if pacman.state.name == 'OK' then
            pacman:set_state ('DYING')
         end
      end
   end

end


function ActorGhost:set_state (name)

   -- Load state data.
   local state = GHOST_STATES[name]
   assert (state, 'Invalid Ghost state: '..name)
   self.state.name = state.name
   self.state.timer = state.timer

   -- Specific state initialization.
   if name == 'WAITING' then
      self.pos = { i=self.start_pos.i, j=self.start_pos.j }
      self.barriers = { 'wall', 'door' }

   elseif name == 'OK' then
      self.barriers = { 'wall' }

   elseif name == 'FROZEN' then
      -- Nothing to do.

   elseif name == 'DEAD' then
      self.dir = 'NONE'
   end
end


function ActorGhost:reset ()
   self.dir = 'NONE'
   self.pos = { i = self.start_pos.i, j = self.start_pos.j }
   self:set_state ('WAITING')
end


-- Updates Ghost direction using a.i.

function ActorGhost:update_dir ()
   if self.state.name == 'DEAD' then return end

   -- Change possible dirs array to a more convenient format.
   local _dirs = self:get_possible_dirs ()
   if #_dirs == 0 then self.dir = 'NONE' return end
   local dirs = {}
   local ndirs = 0
   for i = 1, #_dirs do
      dirs [_dirs[i]] = true
      ndirs = ndirs + 1
   end

   -- Don't do 180 inversions unless is the only possible move.
   local inverse =
      function (dir)
         inv = { ['NORTH'] = 'SOUTH', ['SOUTH'] = 'NORTH',
                 ['WEST'] ='EAST', ['EAST'] = 'WEST' }
         return inv[dir]
      end
   if ndirs > 1 and self.dir ~= 'NONE' then
      dirs[inverse(self.dir)] = nil
   end

   -- Update direction.
   local pacman = self.board:get_pacman ()
   local dir = nil

   -- Run away from Pacman.
   if self.state.name == 'FROZEN' then
      if pacman.pos.i > self.pos.i and dirs['NORTH'] then
         dir = 'NORTH'
      elseif pacman.pos.i < self.pos.i and dirs['SOUTH'] then
         dir = 'SOUTH'
      elseif pacman.pos.j > self.pos.j and dirs['WEST'] then
         dir = 'WEST'
      elseif pacman.pos.j < self.pos.j and dirs['EAST'] then
         dir = 'EAST'
      end

   -- Chase him.
   else
      if pacman.pos.i > self.pos.i and dirs['SOUTH'] then
         dir = 'SOUTH'
      elseif pacman.pos.i < self.pos.i and dirs['NORTH'] then
         dir = 'NORTH'
      elseif pacman.pos.j > self.pos.j and dirs['EAST'] then
         dir = 'EAST'
      elseif pacman.pos.j < self.pos.j and dirs['WEST'] then
         dir = 'WEST'
      end
   end

   -- Make random move.
   local salt = function () return math.random (1, 3) == 3 end
   if dir == nil or salt() then
      local names = {'NORTH', 'SOUTH', 'WEST', 'EAST'}
      while true do
         dir = names[math.random (1, #names)]
         if dirs[dir] then
            break
         end
      end
   end
   self.dir = dir
end


function ActorGhost:update_state ()

   -- Update state timer.
   local timer = self.state.timer
   if timer and timer > 0 then
      self.state.timer = timer - 1
      return
   end

   -- Change state.
   local name = self.state.name
   if name == 'OK' then
      -- Nothing to do.

   -- Now Ghost can transpose doors.
   elseif name == 'WAITING' then
      self:set_state ('OK')

   -- Unfreeze Ghost.
   elseif name == 'FROZEN' then
      self:set_state ('OK')

   -- Re-spawn Ghost.
   elseif name == 'DEAD' then
      self:set_state ('WAITING')
   end
end

-- End: actor_ghost.lua
