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
         object = {},
      }
   end

   mt._detachData = function (self)
      trace ('_detachData')
      mt[self] = nil
   end

   mt._addObject = function (self, obj)
      trace ('_addObject(%s)', obj:getId ())
      mt[self].object[obj:getId ()] = obj
   end

   mt._removeObject = function (self, obj)
      trace ('_removeObject(%s)', obj:getId ())
      mt[self].object[obj:getId ()] = nil
   end
end
