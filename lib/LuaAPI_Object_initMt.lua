do
   local mt = ...
   local trace = mt._trace
   mt._traceSelf = function (self) return mt[self]._id end
   -- mt._traceOff._init        = true
   -- mt._traceOff._fini        = true
   mt._traceOff._addEvent    = true
   mt._traceOff._removeEvent = true
   mt._traceOff.getDocument  = true
   mt._traceOff.getType      = true
   mt._traceOff.getId        = true
   mt._traceOff.getParents   = true
   mt._traceOff.getEvents    = true
   mt._traceOff.getEvent     = true
   mt._traceOff.getLambda    = true
   mt._traceOff.createEvent  = true

   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, doc, type, id, data, funcs)
      local data = data or {}
      local funcs = funcs or {}

      -- Private data.
      data._document     = assert (doc)  -- the container document
      data._type         = assert (type) -- object type
      data._id           = assert (id)   -- object id
      data._attribution  = {}            -- attribution evts indexed by id
      data._presentation = {}            -- presentation evts indexed by id
      data._selection    = {}            -- selection evts indexed by id

      local get_data_attr = function ()
         return data._attribution
      end
      local get_data_pres = function ()
         return data._presentation
      end
      local get_data_seln = function ()
         return data._selection
      end

      -- Getters & setters.
      funcs.document     = {mt.getDocument, nil}
      funcs.type         = {mt.getType,     nil}
      funcs.id           = {mt.getId,       nil}
      funcs.parents      = {mt.getParents,  nil} -- deprecated
      funcs.events       = {mt.getEvents,   nil}
      funcs.lambda       = {mt.getLambda,   nil}
      --
      funcs.attribution  = {get_data_attr,  nil}
      funcs.presentation = {get_data_pres,  nil}
      funcs.selection    = {get_data_seln,  nil}

      return saved_attachData (self, data, funcs)
   end

   -- Initialization.
   local saved_init = assert (mt._init)
   mt._init = function (self)
      trace (self, '_init')
      self.document:_addObject (self)
      assert (self:createEvent ('presentation', '@lambda'))
      return saved_init (self)
   end

   -- Finalization.
   local saved_fini = assert (mt._fini)
   mt._fini = function (self)
      trace (self, '_fini')
      self.document:_removeObject (self)
      return saved_fini (self)
   end

   -- Adds event to object.
   mt._addEvent = function (self, evt)
      trace (self, '_addEvent', evt.id)
      mt[self]['_'..evt.type][evt.id] = evt
   end

   -- Removes event from object.
   mt._removeEvent = function (self, evt)
      trace (self, '_removeEvent', evt.id)
      mt[self]['_'..evt.type][evt.id] = nil
   end

   -- Object::getDocument().
   mt.getDocument = function (self)
      trace (self, 'getDocument')
      return mt[self]._document
   end

   -- Object::getType().
   mt.getType = function (self)
      trace (self, 'getType')
      return mt[self]._type
   end

   -- Object::getId().
   mt.getId = function (self)
      trace (self, 'getId')
      return mt[self]._id
   end

   -- Object::getParents().
   mt.getParents = function (self)
      trace (self, 'getParents')
      return self.document:_getParents (self)
   end

   -- Object::getEvents().
   mt.getEvents = function (self)
      trace (self, 'getEvents')
      local t = {}
      for _,tp in ipairs {'attribution', 'presentation', 'selection'} do
         for _,v in pairs (self[tp]) do
            table.insert (t, v)
         end
      end
      return t
   end

   -- Object::getEvent().
   mt.getEvent = function (self, type, id)
      trace (self, 'getEvent', type, id)
      if type == 'attribution' then
         return self.attribution[id]
      elseif type == 'presentation' then
         return self.presentation[id]
      elseif type == 'selection' then
         return self.selection[id]
      else
         error ('should not get here')
      end
   end

   -- Object::getLambda().
   mt.getLambda = function (self, type, id)
      trace (self, 'getLambda', type, id)
      return self.presentation['@lambda']
   end

   -- Object::createEvent().
   mt.createEvent = function (self, type, id)
      trace (self, "createEvent", type, id)
      return self.document:createEvent (type, self, id)
   end
end
