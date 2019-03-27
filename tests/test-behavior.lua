local assert    = assert
local coroutine = coroutine
local error     = error
local print     = print

local behavior = require'behavior'
_ENV = nil

-- _new
-- do
--    local bhv = behavior._new (function()end, 'init')
--    assert (bhv.co)
--    assert (bhv.name == 'init')
--    assert (bhv.parent == nil)
--    assert (bhv.root == bhv)
--    assert (bhv:_getBehavior (bhv.co) == bhv)
-- end

-- spawn
do
   local bhv = behavior.init ()
   bhv:spawn {
      function ()
         print'<x start>'
         bhv:await'B'
         print'<x end>'
      end,
      function ()
         print'<y start>'
         bhv:emit'A'
         print'<y end>'
      end,
      function ()
         print'<z start>'
         bhv:emit'B'
         bhv:await'C'
         print'<z end>'
      end
   }
end
