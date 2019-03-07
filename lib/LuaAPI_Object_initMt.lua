do
   local mt = ...
   local trace = mt._trace

   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, data, funcs)
      local data = data or {}
      local funcs = funcs or {}

      -- Private data.
      data._attribution  = {}   -- attribution evts indexed by id
      data._presentation = {}   -- presentation evts indexed by id
      data._selection    = {}   -- selection evts indexed by id

      local get_data_attr = function ()
         return assert (data._attribution)
      end
      local get_data_pres = function ()
         return assert (data._presentation)
      end
      local get_data_seln = function ()
         return assert (data._selection)
      end

      -- Getters & setters.
      funcs.document     = {mt.getDocument, nil}
      funcs.parent       = {mt.getParent,   nil}
      funcs.type         = {mt.getType,     nil}
      funcs.id           = {mt.getId,       nil}
      --
      funcs.attribution  = {get_data_attr,  nil}
      funcs.presentation = {get_data_pres,  nil}
      funcs.selection    = {get_data_seln,  nil}

      return saved_attachData (self, data, funcs)
   end

   -- Called when evt is added to object.
   mt._addEvent = function (self, evt)
      --trace ('_addEvent (%s, %s)', self.id, evt.id)
      mt[self]['_'..evt.type][evt.id] = evt
      self.document:_addEvent (evt)
   end

   -- Called when evt is removed from object.
   mt._removeEvent = function (self, evt)
      --trace ('_removeEvent (%s, %s)', self.id, evt.id)
      mt[self]['_'..evt.type][evt.id] = nil
      self.document:_removeEvent (evt)
   end

   mt._beforeEventTransition = function (self, evt, from, to, params)
      -- trace ('_beforeEventTransition (%s, %s, %s, %s)',
      --        self.id, evt.id, from, to)
      return true
   end

   mt._afterEventTransition = function (self, evt, from, to, params)
      -- trace ('_afterEventTransition (%s, %s, %s, %s)',
      --        self.id, evt.id, from, to)
      return true
   end
end
