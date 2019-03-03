do
   local mt = ...
   local trace = function (msg, ...)
      print ('>>> '..mt.__name..': '..msg:format (...))
   end

   mt.__index = function (self, k)
      trace ('__index(%s)', k)
      return mt[k] or mt[self][k]
   end

   mt._attachData = function (self)
      trace ('_attachData')
      mt[self] = {
         id     = self:getId (),
         parent = self:getParent (),
         event  = {
            presentation = {},
            selection = {},
            attribution = {},
         },
      }
   end

   mt._detachData = function (self)
      trace ('_detachData')
      mt[self] = nil
   end

   mt._addEvent = function (self, evt)
      trace ('_addEvent(%s)', evt:getId ())
      mt[self].event[evt:getType ()][evt:getId ()] = evt
   end

   mt._removeEvent = function (self, evt)
      trace ('_removeEvent(%s)', evt:getId ())
      mt[self].event[evt:getType ()][evt:getId ()] = nil
   end
end
