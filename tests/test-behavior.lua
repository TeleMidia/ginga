local assert    = assert
local coroutine = coroutine
local error     = error
local print     = print

local behavior = require'behavior'
_ENV = nil

-- _new
do
   local bhv = behavior._new (function()end, 'init')
   assert (bhv.co)
   assert (bhv.name == 'init')
   assert (bhv.parent == nil)
   assert (bhv.root == bhv)
   assert (bhv:getBehavior (bhv.co) == bhv)
end

-- spawn
do
   local init = behavior.init ()
   init:spawn {
      function ()
         print ('--', coroutine.yield ())
      end
   }
   init:_dumpTree ()
   init:_broadcast ('EVT')
end
