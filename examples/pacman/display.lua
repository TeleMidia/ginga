-- $Id: display.lua,v 1.3 2009/03/11 12:50:45 root Exp $

-- display.lua
-- 11mar2009, gflima@telemidia.puc-rio.br
--
-- Game graphics.  This is platform-dependent.

Display = {

   -- READ-ONLY board reference.
   board_ref = nil,

   -- Game board info.
   gameboard = {
      width = nil,
      height = nil,
      pad_top = nil,
      pad_left = nil,
   },

   -- Scoreboard info.
   scoreboard = {
      pad_top = nil,
      height = 20,
      pacman_live = canvas:new ('media/pacman.png'),
   },

   -- Walls grid cache.  Stores specific wall tiles names.
   walls = nil,

   -- Actors animations current frame.
   pacman_frame = 1,
   ghosts_frame = nil,
}

-- Module globals.

-- Screen dimensions.
local SCREEN_WIDTH, SCREEN_HEIGHT = canvas:attrSize()

-- All images must be squares with this size (in pixels).
local TILE_SIZE = 20

-- All animations must have the same number of frames.
local ANIMS_NFRAMES = 2

-- Directions.  (Used as animation keys).
local DIRS = { 'NORTH', 'SOUTH', 'WEST', 'EAST' }

-- Actor images pathname.  `%s' will be replaced by the specific actor name.
local ACTORS_PATHS = {
   ['NORTH'] = 'media/%s_n.png',
   ['SOUTH'] = 'media/%s_s.png',
   ['WEST'] = 'media/%s_w.png',
   ['EAST'] = 'media/%s_e.png',
}

-- Used to convert Ghost numbers to Ghost names.
local GHOSTS_NAMES = { 'ghost_blue', 'ghost_red', 'ghost_purple',
                       'ghost_green', 'ghost_orange', }

-- Pacman animations.
local PACMAN = {}
do
   for _, dir in pairs (DIRS) do
      PACMAN[dir] = canvas:new (ACTORS_PATHS[dir]:gsub('%%s', 'pacman', 1))
   end
end
local PACMAN_DEAD = canvas:new ('media/pacman_dead.png')

-- Ghosts animations.
local GHOSTS = {}
do
   for _, name in pairs (GHOSTS_NAMES) do
      GHOSTS[#GHOSTS + 1] = {}
      for _, dir  in pairs (DIRS) do
         GHOSTS[#GHOSTS][dir] =
            canvas:new (ACTORS_PATHS[dir]:gsub('%%s', name, 1))
      end
   end
end

local GHOST_FROZEN = {}
do
   for _, dir in pairs (DIRS) do
      GHOST_FROZEN[dir] =
         canvas:new (ACTORS_PATHS[dir]:gsub('%%s', 'ghost_frozen', 1))
   end
end

-- Static tiles draws.
local TILES = {
   ['door'] = canvas:new ('media/door.png'),
   ['food'] = canvas:new ('media/food.png'),
   ['freezer'] = canvas:new ('media/freezer.png'),
}
-- Append specific wall tiles.
do
   local suffixes = { '', 't', 'b', 'l', 'r', 'tb', 'lr', 'bl', 'blr', 'br',
                      'tbl', 'tblr', 'tbr', 'tl', 'tlr', 'tr' }
   for i = 1, #suffixes do
      TILES['wall_'..suffixes[i]] = canvas:new ('media/wall_'
                                               ..suffixes[i]..'.png')
   end
end

-- Exported functions.


-- Constructor.

function Display:new (board)
   assert (board, 'Invalid constructor parameters')

   local o = {}
   setmetatable (o, self)
   self.__index = self

   -- Initialize display.
   o.board_ref = board
   o.gameboard.height = #board.grid * TILE_SIZE
   o.gameboard.width = #board.grid[1] * TILE_SIZE

   assert (o.gameboard.height + o.scoreboard.height <= SCREEN_HEIGHT,
           'Board height is larger then screen.')
   assert (o.gameboard.width <= SCREEN_WIDTH,
           'Board width is larger then screen.')
   o.gameboard.pad_left = (SCREEN_WIDTH - o.gameboard.width) / 2
   o.gameboard.pad_top = (SCREEN_HEIGHT
                          - (o.gameboard.height + o.scoreboard.height)) / 2

   -- Initialize scoreboard.
   o.scoreboard.pad_top = o.gameboard.pad_top + o.gameboard.height

   -- Create walls cache.
   local grid = o.board_ref.grid
   local lines = #grid
   local columns = #grid[1]
   o.walls = {}
   for i = 1, lines do
      o.walls[i] = {}
      for j = 1, columns do
         if grid[i][j] == 'wall' then
            local suffix = ''
            local neighbor = nil

            -- Top connection.
            neighbor = grid[i - 1] and grid[i - 1][j]
            if neighbor and neighbor == 'wall' then
               suffix = suffix..'t'
            end

            -- Bottom connection.
            neighbor = grid[i + 1] and grid[i + 1][j]
            if neighbor and neighbor == 'wall' then
               suffix = suffix..'b'
            end

            -- Left connection.
            neighbor = grid[i][j - 1]
            if neighbor and neighbor == 'wall' then
               suffix = suffix..'l'
            end

            -- Right connection.
            neighbor = grid[i][j + 1]
            if neighbor and neighbor == 'wall' then
               suffix = suffix..'r'
            end

            o.walls[i][j] = 'wall_'..suffix
         end
      end
   end

   -- Initialize Ghosts frame counter.
   local ghosts = o.board_ref:get_ghosts ()
   o.ghosts_frame = {}
   for i = 1, #ghosts do
      o.ghosts_frame[i] = 0
   end
   return o
end


-- Draws an animation into gameboard at cell (I,J)

function Display:draw_anim (cvs, frame, i, j)
   local x = self.gameboard.pad_left + (j - 1) * TILE_SIZE
   local y = self.gameboard.pad_top + (i - 1) * TILE_SIZE
   cvs:attrCrop ((TILE_SIZE * frame), 0, TILE_SIZE, TILE_SIZE)
   canvas:compose (x, y, cvs)
end


-- Draws a static image into gameboard at cell (I,J).

function Display:draw_img (cvs, i, j)
   local x = self.gameboard.pad_left + (j - 1) * TILE_SIZE
   local y = self.gameboard.pad_top + (i - 1) * TILE_SIZE
   canvas:compose (x, y, cvs)
end


-- Draws scoreboard.

function Display:draw_scoreboard (level, lives)
   canvas:attrColor('yellow')
   canvas:attrFont('vera', 18)
   local text = 'Nível '.. level
   local textw = canvas:measureText (text)
   local totalw = textw + 80
   local pad_left = self.gameboard.pad_left + 5

   canvas:drawText (pad_left, self.scoreboard.pad_top, text)
   for i = 0, lives - 1 do
      canvas:compose (
         pad_left + textw + 10 + i * 20,
         self.scoreboard.pad_top,
         self.scoreboard.pacman_live)
   end
end


-- Shows win/lose end message.

local function end_message (win)
   -- Fill canvas with an transparent color.
   canvas:attrColor (0, 0, 0, 0)
   canvas:clear ()

   -- Player won.
   if win then
      canvas:attrColor('yellow')
      canvas:attrFont('vera', 18)
      local text1 = 'Parabéns, você ganhou.'
      local text2 = 'Torne o jogo ainda mais desafiador, '
         ..'contribua com novos níveis.'
      local w1, h1 = canvas:measureText (text1)
      local w2, h2 = canvas:measureText (text2)
      local x1 = (SCREEN_WIDTH - w1) / 2
      local y1 = (SCREEN_HEIGHT - h1 - h2) /2
      local x2 = (SCREEN_WIDTH - w2) / 2
      local y2 = y1 + h1 + 5
      canvas:drawText (x1, y1, text1)
      canvas:drawText (x2, y2, text2)

   -- Player lose.
   else
      canvas:attrColor('red')
      canvas:attrFont('vera', 25)
      local text = 'Você perdeu.'
      local w, h = canvas:measureText (text)
      local x = (SCREEN_WIDTH - w) / 2
      local y = (SCREEN_HEIGHT - h) /2
      canvas:drawText (x, y, text)
   end
   canvas:flush ()
end
Display.end_message = end_message


-- Redraw everything.

function Display:redraw (level, lives)
   -- Fill canvas with black.
   canvas:attrColor ('black')
   canvas:clear ()

   -- Draws gameboard.
   local grid = self.board_ref.grid
   for i = 1, #grid do
      for j = 1, #grid[1] do
         local s = grid[i][j]

         if s ~= 'empty' then
            if s == 'wall' then         -- Get specific wall tile.
               s = self.walls[i][j]
            end
            self:draw_img (TILES[s], i, j)
         end
      end
   end

   -- Draw Pacman.
   local pacman = self.board_ref:get_pacman ()
   local dir = pacman.dir
   if dir == nil or dir == 'NONE' then dir = 'EAST' end
   if pacman.state.name == 'OK' then
      self:draw_anim (PACMAN[dir], self.pacman_frame,
                      pacman.pos.i, pacman.pos.j)
      self.pacman_frame = (self.pacman_frame + 1) % ANIMS_NFRAMES
   else
      self:draw_img (PACMAN_DEAD, pacman.pos.i, pacman.pos.j)
   end

   -- Draw Ghosts.
   local ghosts = self.board_ref:get_ghosts ()
   for i = 1, #ghosts do
      if ghosts[i].state.name ~= 'DEAD' then
         local dir = ghosts[i].dir
         if dir == nil or dir == 'NONE' then dir = 'NORTH' end
         local cvs
         if ghosts[i].state.name == 'FROZEN' then
            cvs = GHOST_FROZEN[dir]
         else
            cvs = GHOSTS[(i%#GHOSTS) + 1][dir]
         end
         self:draw_anim (cvs, self.ghosts_frame[i],
                         ghosts[i].pos.i, ghosts[i].pos.j)
         self.ghosts_frame[i] = (self.ghosts_frame[i] + 1) % ANIMS_NFRAMES
      end
   end
   self:draw_scoreboard (level, lives)
   canvas:flush ()
end

-- End: display.lua
