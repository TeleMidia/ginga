local assert    = assert
local await     = coroutine.yield
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

      -- Private data.
      data._ports = {}

      -- Getters & setters.
      funcs.ports = {mt.getPorts, nil}

      return saved_attachData (self, doc, type, id, data, funcs)
   end

   -- Initializes context.
   local saved_init = assert (mt._init)
   mt._init = function (self)
      saved_init (self)
      --
      -- Default behavior.
      self:run {
         function ()            -- start lambda
            while true do
               await {event=self.lambda, transition='start'}
               print ('context', self.id, 'lambda start')
               for _,port in ipairs (self.ports) do
                  local evt = self.document:getEvent (port)
                  if evt then
                     evt:transition ('start')
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
   mt.addPort = function (self, evt)
      if type (evt) == 'userdata' then
         evt = evt:getQualifiedId ()
      end
      if self.document:getEvent (evt) == nil then
         return false           -- no such event
      end
      table.insert (mt[self]._ports, evt)
      return true
   end

   -- Context::removePort()
   mt.removePort = function (self, evt)
      if type (evt) == 'userdata' then
         evt = evt:getQualifiedId ()
      end
      local t = assert (mt[self]._ports)
      for i=1,#t do
         if t[i] == evt then
            table.remove (t, i)
            return true
         end
      end
      return false              -- no such event
   end
end
