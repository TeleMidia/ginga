-- $Id: parser.lua,v 1.2 2009/03/11 12:17:39 root Exp $

-- parser.lua
-- 11mar2009, gflima@telemidia.puc-rio.br
--
-- Level parser.  Takes a level file as input and constructs a level grid.

Parser = {}

-- Symbol used for pacman.  This must appear only once in the level string.
local PACMAN = 'c'

-- Char mappings.
local NAMES = {
   [PACMAN] = 'pacman',
   ['g'] = 'ghost',
   ['.'] = 'food',
   ['*'] = 'freezer',
   ['/'] = 'empty',
   ['x'] = 'door',
   ['+'] = 'wall'
}
-- Wall synonyms.
do
   NAMES['-'] = NAMES['+']
   NAMES['|'] = NAMES['+']
end

-- All symbols used in NAMES.
local SYMBOLS = ''
do
 for s, _ in pairs (NAMES) do
      -- Escape special chars.  We're going to use these inside regexps.
      if s:find('[^%a%d]') then
         s = '%'..s
      end
      SYMBOLS = SYMBOLS..s
   end
end


-- Remove non-mapped symbols from STR.  Returns the cleaned string.

local function clean (str)
   if str == nil then return nil end
   str = str:gsub ('#.-\n', '\n')
   str = str:gsub('[^' .. SYMBOLS .. '\n]', '')

   -- Remove empty lines.
   str = str:gsub ('\n+', '\n')
   if str:find ('\n') == 1 then
      str = str:sub (2)
   end
   return str
end


-- Split STR into lines.  Returns the lines table.

local function lines (str)
   return { str:match ((str:gsub ("[^\n]*\n", "([^\n]*)\n"))) }
end


-- Parse level STR.  Returns level grid.

local function _parse (str)
   str = clean (str)
   assert (str and str ~= '', 'Invalid level string: string is empty')

   local lines = lines (str)

   -- Check level size.
   local n = #lines[1]
   for i = 2, #lines do
      assert (#lines[i] == n, 'Invalid level size: check grid line '..i)
   end

   -- Create level grid.
   local grid = {}
   local pacman_found = false
   for i = 1, #lines do
      grid[i] = {}
      for j = 1, #lines[i] do
         local c = lines[i]:sub (j, j)
         grid[i][j] = NAMES[c]

         -- Check for a single Pacman instance.
         if c == PACMAN then
            assert (pacman_found == false,
                    ('Pacman (%s,%s) appears more than once')
                    :format (i, j))
            pacman_found = true
         end
      end
   end
   assert (pacman_found, 'Pacman not found')
   return grid
end

-- Exported functions.


-- Parse level file.  Returns the level grid.

local function parse (pathname)
   local f = io.open(pathname, 'r')
   local str = f and f:read ("*a")
   assert (str, 'Failed to load level file')
   return _parse (str)
end
Parser.parse = parse

-- End: parser.lua
