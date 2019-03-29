local assert    = assert
local coroutine = coroutine
local ipairs    = ipairs
local print     = print
local table     = table
local type      = type
_ENV = nil

-- Context metatable.
do
   local mt = ...

   -- Attaches private data and access functions
   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, doc, type, id, data, funcs)
      local data = data or {}
      local funcs = funcs or {}
      --
      -- Private data.
      data._ports = {}
      --
      -- Access functions.
      funcs.ports = {mt.getPorts, nil}
      --
      return saved_attachData (self, doc, type, id, data, funcs)
   end

   -- Initializes context.
   local saved_init = assert (mt._init)
   mt._init = function (self)
      saved_init (self)
      --
      -- Default behavior.
      local doc = self.document
      self:spawn {
         function ()            -- start lambda
            while true do
               doc:_await {statemachine=self.lambda, transition='start'}
               for _,port in ipairs (self.ports) do
                  local sm = doc:getStateMachine (port)
                  if sm then
                     sm:transition'start'
                  end
               end
            end
         end
      }
   end

   -- Exported functions ---------------------------------------------------

   -- Context::getPorts().
   mt.getPorts = function (self)
      local t = {}
      for _,v in ipairs (assert (mt[self]._ports)) do
         table.insert (t, v)
      end
      return t
   end

   -- Context::addPort().
   mt.addPort = function (self, sm)
      if type (sm) == 'userdata' then
         sm = sm:getQualifiedId ()
      end
      if self.document:getStateMachine (sm) == nil then
         return false           -- no such port
      end
      table.insert (mt[self]._ports, sm)
      return true
   end

   -- Context::removePort()
   mt.removePort = function (self, sm)
      if type (sm) == 'userdata' then
         sm = sm:getQualifiedId ()
      end
      local t = assert (mt[self]._ports)
      for i=1,#t do
         if t[i] == sm then
            table.remove (t, i)
            return true
         end
      end
      return false              -- no such port
   end
end
