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
      trace ('_attachData (%s)', self:getId ())
      mt[self] = {
         doc    = self:getDocument (),
         parent = self:getParent (),
         type   = self:getType (),
         id     = self:getId (),
         event  = {
            presentation = {},  -- presentation evts indexed by id
            selection = {},     -- selection evts indexed by id
            attribution = {},   -- attribution evts indexed by id
         },
      }
   end

   mt._detachData = function (self)
      trace ('_detachData (%s)', self:getId ())
      mt[self] = nil
   end

   -- Called when evt is added to object.
   mt._addEvent = function (self, evt)
      trace ('_addEvent (%s, %s)', self:getId (), evt:getId ())
      mt[self].event[evt:getType ()][evt:getId ()] = evt
      self:getDocument ():_addEvent (evt)
   end

   -- Called when evt is removed from object.
   mt._removeEvent = function (self, evt)
      trace ('_removeEvent (%s, %s)', self:getId (), evt:getId ())
      mt[self].event[evt:getType ()][evt:getId ()] = nil
      self:getDocument ():_removeEvent (evt)
   end
end
