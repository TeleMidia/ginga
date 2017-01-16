-- $Id: actor_pacman.lua,v 1.2 2009/03/11 12:00:39 root Exp $

-- actor_pacman.lua
-- 11mar2009, gflima@telemidia.puc-rio.br
--
-- Holds Pacman data and control logic.

require 'actor'

ActorPacman = Actor:new ()

local PACMAN_STATES = {

   -- Pacman is fine.  (This is the initial state.)
   ['OK'] = { name = 'OK' },

   -- Pacman is going to die.
   ['DYING'] = {
      name = 'DYING',
      timer = 20,             -- When reaches 0 indicates a state change.
   },

   -- Pacman is dead.
   ['DEAD'] = { name = 'DEAD' },
}

-- Actor interface implementation.


function ActorPacman:new (i, j, board)
   assert (i > 0 and j > 0 and board, 'Invalid constructor parameters')
   local o = {}
   setmetatable (o, self)
   self.__index = self

   -- Initialize Pacman.
   o.is_pacman = true
   o.dir = 'NONE'
   o.start_pos = { i=i, j=j }
   o.pos = { i=i, j=j }
   o.barriers = {'wall', 'door'}
   o.board = board
   o.state = {}; o:set_state ('OK')
   return o
end


function ActorPacman:collect ()
   if self.state.name ~= 'OK' then return end

   local i, j = self.pos.i, self.pos.j
   local tile = self.board.grid[i][j]

   -- Collect food.
   if tile == 'food' then
      self.board.food = self.board.food - 1
      self.board.grid[i][j] = 'empty'

   -- Collect freezer.
   elseif tile == 'freezer' then

      -- Freeze all non-dead Ghosts.
      local ghosts = self.board:get_ghosts ()
      for i = 1, #ghosts do
         if ghosts[i].state.name ~= 'DEAD' then
            ghosts[i]:set_state ('FROZEN')
         end
      end

      -- Consume freezer.
      self.board.food = self.board.food - 1
      self.board.grid[i][j] = 'empty'
   end
end


function ActorPacman:collisions ()
   if self.state.name ~= 'OK' then return end

   -- Delegates collision checks to Ghosts.
   local ghosts = self.board:get_ghosts ()
   for i = 1, #ghosts do
      if ghosts[i].state.name ~= 'DEAD' then
         ghosts[i]:collisions ()
      end
   end
end


function ActorPacman:set_state (name)

   -- Load state data.
   local state = PACMAN_STATES[name]
   assert (state, 'Invalid Pacman state: '..name)
   self.state.name = state.name
   self.state.timer = state.timer

   -- Specific state initialization.
   if name == 'OK' then
      -- Nothing to do.

   elseif name =='DYING' then
      self.dir = 'NONE'

   elseif name == 'DEAD' then
      -- Nothing to do.
   end
end


function ActorPacman:reset ()
   self.dir = 'NONE'
   self.pos = { i = self.start_pos.i, j = self.start_pos.j }
   self:set_state ('OK')
end


-- Updates Pacman direction using user input.

require 'input'
function ActorPacman:update_dir ()
   if self.state.name ~= 'OK' then return end

   -- Get input from user.
   local dir = Input.get_dir ()
   if dir == self.dir then
      return                       -- Nothing to do.
   end

   -- Updates direction.
   local dirs = self:get_possible_dirs ()
   for i = 1, #dirs do
      if dir == dirs[i] then
         self.dir = dir
         return
      end
   end
end


function ActorPacman:update_state ()

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

   elseif name == 'DYING' then
      self:set_state('DEAD')

   elseif name == 'DEAD' then
      -- Nothing to do.
   end
end

-- End: actor_pacman.lua
