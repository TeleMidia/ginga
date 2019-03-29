local assert    = assert
local coroutine = coroutine
local error     = error
local ipairs    = ipairs
local pairs     = pairs
local print     = print
local type      = type

local behavior = require'Behavior'
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
   --bhv:_dump ()
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
   local called = false
   bhv.error = function (obj, errmsg)
      assert (obj)
      assert (errmsg)
      assert (type (errmsg) == 'string')
      called = true
   end
   bhv:spawn {
      function ()
         error'argh'
      end
   }
   assert (called)
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

-- par ---------------------------------------------------------------------

do
   local bhv = behavior.init ()
   local t = {}
   bhv:parOr {                  -- par/or outside behavior
      function ()
         t[#t+1] = 'x'
      end,
      function ()
         t[#t+1] = 'y'
      end,
   }
   assert (#t == 1)
   assert (t[1] == 'x')

   t = {}
   bhv:parAnd {                 -- par/and outside behavior
      function ()
         t[#t+1] = 'x'
      end,
      function ()
         t[#t+1] = 'y'
      end,
   }
   assert (#t == 2)
   assert (t[1] == 'x')
   assert (t[2] == 'y')

   t = {}
   bhv:spawn {
      function ()
         t[#t+1] = 'x'
         bhv:parOr {            -- par/or inside behavior
            function ()
               t[#t+1] = 'y'
            end,
            function ()
               t[#t+1] = 'z'
            end
         }
         t[#t+1] = 'w'
      end,
   }
   assert (#t == 3)
   assert (t[1] == 'x')
   assert (t[2] == 'y')
   assert (t[3] == 'w')

   t = {}
   bhv:spawn {
      function ()
         t[#t+1] = 'x'
         bhv:parAnd {           -- par/and inside behavior
            function ()
               t[#t+1] = 'y'
            end,
            function ()
               t[#t+1] = 'z'
            end
         }
         t[#t+1] = 'w'
      end,
   }
   assert (#t == 4)
   assert (t[1] == 'x')
   assert (t[2] == 'y')
   assert (t[3] == 'z')
   assert (t[4] == 'w')
end

do
   local bhv = behavior.init {}
   local t = {}

   bhv:parOr {                  -- par/or outside behavior,
      function ()               --   awaiting external emit
         t[#t+1] = 'x'
         bhv:await'A'
         t[#t+1] = 'y'
      end,
      function ()
         t[#t+1] = 'z'
         bhv:await'A'
         t[#t+1] = 'w'
      end
   }
   assert (#t == 2)
   assert (t[1] == 'x')
   assert (t[2] == 'z')

   bhv:spawn {
      function ()
         t[#t+1] = '#'
      end
   }
   assert (#t == 3)
   assert (t[3] == '#')

   bhv:emit'A'
   assert (#t == 4)
   assert (t[4] == 'y')

   t = {}
   bhv:parAnd {                 -- par/and outside behavior,
      function ()               --   awaiting external emit
         t[#t+1] = 'x'
         bhv:await'A'
         t[#t+1] = 'y'
      end,
      function ()
         t[#t+1] = 'z'
         bhv:await'A'
         t[#t+1] = 'w'
      end
   }
   assert (#t == 2)
   assert (t[1] == 'x')
   assert (t[2] == 'z')

   bhv:spawn {
      function ()
         t[#t+1] = '#'
      end
   }
   assert (#t == 3)
   assert (t[3] == '#')

   bhv:emit'A'
   assert (#t == 5)
   assert (t[4] == 'y')
   assert (t[5] == 'w')
end

do
   local bhv = behavior.init {trace=false}
   local t = {}
   bhv:spawn {
      function ()
         t[#t+1] = 'spawn:1'
         bhv:await'A'
         t[#t+1] = 'spawn:2'
         bhv:parAnd {
            function ()
               t[#t+1] = 'and1:1'
               bhv:await'B'
               t[#t+1] = 'and1:2'
               bhv:parOr {
                  function ()
                     t[#t+1] = 'or1:1'
                     bhv:emit'A'
                     t[#t+1] = 'or1:2'
                  end,
                  function ()
                     t[#t+1] = 'or2:1'
                  end,
               }
               t[#t+1] = 'and1:3'
            end,
            function ()
               t[#t+1] = 'and2:1'
               bhv:await'B'
               t[#t+1] = 'and2:2'
            end,
            function ()
               t[#t+1] = 'and3:1'
               bhv:emit'B'
               t[#t+1] = 'and3:2'
               bhv:await'A'
               t[#t+1] = 'and3:3'
            end
         }
         t[#t+1] = 'spawn:3'
      end,
   }
   assert (#t == 1)
   assert (t[1] == 'spawn:1')

   bhv:emit'A'
   assert (t[2] == 'spawn:2')
   assert (t[3] == 'and1:1')
   assert (t[4] == 'and2:1')
   assert (t[5] == 'and3:1')
   assert (t[6] == 'and1:2')
   assert (t[7] == 'or1:1')
   assert (t[8] == 'or2:1')
   assert (t[9] == 'and1:3')
   assert (t[10] == 'and2:2')
   assert (t[11] == 'and3:2')

   bhv:emit'A'
   assert (t[12] == 'and3:3')
   assert (t[13] == 'spawn:3')
end
