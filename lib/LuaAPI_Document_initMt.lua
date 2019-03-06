do
   local mt = ...
   local trace = mt._trace

   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self)
      local data = {
         _object  = {},
         _event   = {},
      }
      local get_data_object = function ()
         return assert (data._object)
      end
      local get_data_event = function ()
         return assert (data._event)
      end
      local funcs = {
         objects = {mt.getObjects,   nil},
         object  = {get_data_object, nil},
         event   = {get_data_event,  nil},
      }
      return saved_attachData (self, data, funcs)
   end

   -- Called when obj is added to document.
   mt._addObject = function (self, obj)
      trace ('_addObject (%s)', obj.id)
      mt[self]._object[obj.id] = obj
   end

   -- Called when obj is removed from document.
   mt._removeObject = function (self, obj)
      trace ('_removeObject (%s)', obj.id)
      mt[self]._object[obj.id] = nil
   end

   -- Called when evt is added to some object.
   mt._addEvent = function (self, evt)
      trace ('_addEvent (%s)', evt.id)
      mt[self]._event[evt.qualifiedId] = evt
   end

   -- Called when evt is removed from some object.
   mt._removeEvent = function (self, evt)
      trace ('_removeEvent (%s)', evt.id)
      mt[self]._event[evt.qualifiedId] = nil
   end

   -- Parses the given qualified id.
   -- This can be called as a method or as an ordinary function.
   mt._parseQualifiedId = function  (self, id)
      local id = id or self
      trace ('parseQualifiedId (%s)', id)
      local tp, o, e

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
      assert (tp)
      assert (o)
      assert (e)
      return tp, o, e
   end

   -- Implementation of the 2nd version of Document::createEvent().
   local saved_mt_createEvent = mt.createEvent
   mt.createEvent = function (self, tp, objId, evtId)
      trace ('createEvent (%s, %s, %s)', tp, objId, evtId)
      if objId == nil and evtId == nil then
         tp, objId, evtId = self:_parseQualifiedId (tp)
      end
      return saved_mt_createEvent (self, tp, objId, evtId)
   end

   -- Implementation of Document::getEvent().
   mt.getEvent = function (self, id)
      trace ('getEvent (%s)', id)
      local tp, o, e = self:_parseQualifiedId (id)
      if tp == nil then
         return nil             -- bad format
      end

      local obj = self.object[o]
      if not obj then
         return nil             -- no such object
      end

      return obj:getEvent (tp, e)
   end
end
