do
   local mt = ...
   local trace = function (msg, ...)
      print ('>>> '..mt.__name..': '..msg:format (...))
   end

   mt.__index = function (self, k)
      --trace ('__index(%s)', k)
      return mt[k] or mt[self][k]
   end

   mt._attachData = function (self)
      trace ('_attachData ()')
      mt[self] = {
         object = {},           -- objects indexed by id
         event  = {},           -- events indexed by qualified id
      }
   end

   mt._detachData = function (self)
      trace ('_detachData ()')
      mt[self] = nil
   end

   -- Called when obj is added to document.
   mt._addObject = function (self, obj)
      trace ('_addObject (%s)', obj:getId ())
      mt[self].object[obj:getId ()] = obj
   end

   -- Called when obj is removed from document.
   mt._removeObject = function (self, obj)
      trace ('_removeObject (%s)', obj:getId ())
      mt[self].object[obj:getId ()] = nil
   end

   -- Called when evt is added to some object.
   mt._addEvent = function (self, evt)
      trace ('_addEvent (%s)', evt:getId ())
      mt[self].event[evt:getQualifiedId ()] = evt
   end

   -- Called when evt is removed from some object.
   mt._removeEvent = function (self, evt)
      trace ('_removeEvent (%s)', evt:getId ())
      mt[self].event[evt:getQualifiedId ()] = nil
   end

   -- Lua implementation of Document::getEvent().
   mt.getEvent = function (self, id)
      trace ('getEvent (%s)', id)
      local o, e, tp

      o, e = id:match ('([%w_-]+)@([%w_-]+)')
      if o and e then
         if e == 'lambda' then
            e = '@lambda'
         end
         tp = 'presentation'
         goto tail
      end

      o, e = id:match ('([%w_-]+)%.([%w_-]+)')
      if o and e then
         tp = 'attribution'
         goto tail
      end

      o, e = id:match ('([%w_-]+)<([%w_-]+)>')
      if o and e then
         tp = 'selection'
      else
         return nil             -- bad format
      end

      ::tail::
      local obj = self.object[o]
      if not obj then
         return nil             -- no such object
      end

      return obj.event[tp][e]
   end
end
