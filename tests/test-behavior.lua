local assert    = assert
local coroutine = coroutine
local error     = error
local ipairs    = ipairs
local pairs     = pairs
local print     = print

local behavior = require'behavior'
_ENV = nil

-- init --------------------------------------------------------------------
do
   local bhv = behavior.init ()
   assert (bhv.co)
   assert (bhv.name == 'init')
   assert (bhv.parent == nil)
   assert (bhv.root == bhv)
   assert (bhv:_getBehavior (bhv.co) == bhv)
end

-- spawn -------------------------------------------------------------------
do
   local bhv = behavior.init ()
   bhv:spawn {}
   bhv:emit'a'
   assert (true)
end

do
   local bhv = behavior.init ()
   local t = {}
   bhv:spawn {
      function ()
         t[#t+1] = 'x'
      end,
      function ()
         t[#t+1] = 'y'
      end,
      function ()
         t[#t+1] = 'z'
      end
   }
   assert (#t == 3)
   assert (t[1] == 'x')
   assert (t[2] == 'y')
   assert (t[3] == 'z')
end

do
   local bhv = behavior.init ()
   -- bhv.trace = true
   local t = {}
   bhv:spawn {
      function ()
         t[#t+1] = 'x'
         bhv:await {}
      end,
      function ()
         t[#t+1] = 'y'
         bhv:spawn {            -- nested spawn (delayed to next cycle)
            function ()
               t[#t+1] = 'z1'
            end,
            function ()
               t[#t+1] = 'z2'
               bhv:await {}
            end,
         }
         t[#t+1] = 'w'
      end,
   }
   assert (#t == 5)
   assert (t[1] == 'x')
   assert (t[2] == 'y')
   assert (t[3] == 'w')
   assert (t[4] == 'z1')
   assert (t[5] == 'z2')
end

do
   local bhv = behavior.init ()
   -- bhv.trace = true
   local t = {}
   bhv.error = print
   bhv:spawn {
      function ()
         error'argh'
      end
   }
end

-- await/emit --------------------------------------------------------------
do
   local bhv = behavior.init ()
   local t = {}
   bhv:spawn {
      function ()
         --print'x start'
         bhv:await'A'
         bhv:await'B'
         --print'x end'
         t[#t+1] = 'x'
      end,
      function ()
         --print'y start'
         bhv:await'A'
         --print'y end'
         t[#t+1] = 'y'
      end,
      function ()
         --print'z start'
         bhv:emit'B'
         bhv:await'B'
         --print'z end'
         t[#t+1] = 'z'
      end,
   }
   assert (#t == 0)

   bhv:emit'A'
   assert (#t == 1)
   assert (t[1] == 'y')

   bhv:emit'B'
   assert (#t == 3)
   assert (t[1] == 'y')
   assert (t[2] == 'x')
   assert (t[3] == 'z')
end
