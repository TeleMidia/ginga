-- Parses qualified id.
--
-- Returns the resulting event type, object id, and event id if successful;
-- otherwise returns nil.
--
local parseQualifiedId = function (id)
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

-- Initialize document metatable.
do
   local mt = ...

   local trace = mt._trace
   mt._traceOff.getObjects = true
   mt._traceOff.getRoot    = true

   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, data, funcs)
      local data = data or {}
      local funcs = funcs or {}

      -- Private data.
      data._object = {}         -- objects indexed by id
      data._event  = {}         -- events indexed by id

      local get_data_object = function ()
         return assert (data._object)
      end
      local get_data_event = function ()
         return assert (data._event)
      end

      -- Getters & setters.
      funcs.objects  = {mt.getObjects,   nil}
      funcs.root     = {mt.getRoot,      nil}
      funcs.settings = {mt.getSettings,  nil}
      --
      funcs.object   = {get_data_object, nil}
      funcs.event    = {get_data_event,  nil}

      return saved_attachData (self, data, funcs)
   end

   -- Adds object to document.
   mt._addObject = function (self, obj)
      trace (self, '_addObject', obj.id)
      mt[self]._object[obj.id] = obj
   end

   -- Removes object from document.
   mt._removeObject = function (self, obj)
      trace (self, '_removeObject', obj.id)
      mt[self]._object[obj.id] = nil
   end

   -- Adds event to document.
   mt._addEvent = function (self, evt)
      trace (self, '_addEvent', evt.qualifiedId)
      mt[self]._event[evt.qualifiedId] = evt
   end

   -- Removes event from document.
   mt._removeEvent = function (self, evt)
      trace (self, '_removeEvent', evt.qualifiedId)
      mt[self]._event[evt.qualifiedId] = nil
   end

   -- Workhorse of Document::createEvent().
   local saved_createEvent = assert (mt._createEvent)
   mt._createEvent = function (self, type, objId, evtId)
      trace (self, '_createEvent', type, objId, evtId)
      return saved_createEvent (self, type, objId, evtId)
   end

   -- Document::getObjects().
   mt.getObjects = function (self, tmask)
      trace (self, 'getObjects', tmask)
      local t = {}
      for _,v in pairs (mt[self]._object) do
         if tmask == nil or (type (tmask == 'table') and tmask[v.type]) then
            table.insert (t, v)
         end
      end
      return t
   end

   -- Document::getObject().
   mt.getObject = function (self, id)
      trace (self, 'getObject', id)
      return mt[self]._object[id]
   end

   -- Document::getEvent().
   mt.getEvent = function (self, id)
      trace (self, 'getEvent', id)
      local tp, o, e = parseQualifiedId (id)
      if tp == nil then
         return nil             -- bad format
      end
      local obj = self.object[o]
      if not obj then
         return nil             -- no such object
      end
      return obj:getEvent (tp, e)
   end

   -- Document::getRoot().
   mt.getRoot = function (self)
      trace (self, 'getRoot')
      return mt[self]._object.__root__
   end

   -- Document::getSettings ().
   mt.getSettings = function (self)
      trace (self, 'getSettings')
      return mt[self]._object.__settings__
   end

   -- Document::createObject().
   local saved_createObject = assert (mt.createObject)
   mt.createObject = function (self, tp, parent, id)
      trace (self, 'createObject', tp, parent, id)
      return saved_createObject (self, tp, parent, id)
   end

   -- Document::createEvent().
   mt.createEvent = function (self, tp, objId, evtId)
      trace (self, 'createEvent', tp, objId, evtId)
      if objId == nil and evtId == nil then
         tp, objId, evtId = parseQualifiedId (tp)
      end
      return self:_createEvent (tp, objId, evtId)
   end
end
