do
   local mt = ...
   local trace = mt._trace
   mt._traceSelf = function (self) return self.id end
   mt._traceOff._addEvent    = true
   mt._traceOff._removeEvent = true

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

   -- Adds event to object.
   mt._addEvent = function (self, evt)
      trace (self, '_addEvent', evt.id)
      mt[self]['_'..evt.type][evt.id] = evt
      self.document:_addEvent (evt)
   end

   -- Removes event from object.
   mt._removeEvent = function (self, evt)
      trace (self, '_removeEvent', evt.id)
      mt[self]['_'..evt.type][evt.id] = nil
      self.document:_removeEvent (evt)
   end
end
