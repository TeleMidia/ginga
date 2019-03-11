-- Object metatable.
do
   local mt = ...

   -- Attaches private data and access functions.
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
      data._property     = {}            -- property table.

      local get_data_attr = function ()
         return data._attribution
      end
      local get_data_pres = function ()
         return data._presentation
      end
      local get_data_seln = function (...)
         return data._selection
      end
      local get_data_prop = function (...)
         return data._property
      end

      -- Getters & setters.
      funcs.document     = {mt.getDocument, nil}
      funcs.type         = {mt.getType,     nil}
      funcs.id           = {mt.getId,       nil}
      funcs.parents      = {mt.getParents,  nil}
      funcs.events       = {mt.getEvents,   nil}
      funcs.lambda       = {mt.getLambda,   nil}
      funcs.property     = {get_data_prop,  nil}
      --
      funcs.attribution  = {get_data_attr,  nil}
      funcs.presentation = {get_data_pres,  nil}
      funcs.selection    = {get_data_seln,  nil}

      return saved_attachData (self, data, funcs)
   end

   -- Initializes private data.
   local saved_init = assert (mt._init)
   mt._init = function (self)
      self.document:_addObject (self)
      assert (self:createEvent ('presentation', '@lambda'))
      return saved_init (self)
   end

   -- Finalizes private data.
   local saved_fini = assert (mt._fini)
   mt._fini = function (self)
      self.document:_removeObject (self)
      return saved_fini (self)
   end

   -- Adds event to object.
   mt._addEvent = function (self, evt)
      mt[self]['_'..evt.type][evt.id] = evt
   end

   -- Removes event from object.
   mt._removeEvent = function (self, evt)
      mt[self]['_'..evt.type][evt.id] = nil
   end

   -- Gets a string representation of object.
   mt.__tostring = function (self)
      return mt[self]._id
   end

   -- Exported functions ---------------------------------------------------

   -- Object::isComposition().
   mt.isComposition = function (self)
      error ('should be implemented by subclasses')
   end

   -- Object::getDocument().
   mt.getDocument = function (self)
      return mt[self]._document
   end

   -- Object::getType().
   mt.getType = function (self)
      return mt[self]._type
   end

   -- Object::getId().
   mt.getId = function (self)
      return mt[self]._id
   end

   -- Object::getParents().
   mt.getParents = function (self)
      return self.document:_getParents (self)
   end

   -- Object::getEvents().
   mt.getEvents = function (self)
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
      return self.presentation['@lambda']
   end

   -- Object::getProperties().
   mt.getProperties = function (self)
      return mt[self]._property -- FIXME: Clone?
   end

   -- Object::getProperty().
   mt.getProperty = function (self, name)
      return mt[self]._property[name]
   end

   -- Object::setProperty().
   mt.setProperty = function (self, name, value)
      mt[self]._property[name] = value
   end

   -- Object::createEvent().
   mt.createEvent = function (self, type, id)
      return self.document:createEvent (type, self, id)
   end
end
