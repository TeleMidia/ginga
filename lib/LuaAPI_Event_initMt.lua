do
   local mt = ...

   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, data, funcs)
      local data  = data or {}
      local funcs = funcs or {}

      -- Getters & setters.
      funcs.object      = {mt.getObject,      nil}
      funcs.type        = {mt.getType,        nil}
      funcs.id          = {mt.getId,          nil}
      funcs.qualifiedId = {mt.getQualifiedId, nil}
      funcs.state       = {mt.getState,       mt.setState}
      funcs.beginTime   = {mt.getBeginTime,   mt.setBeginTime}
      funcs.endTime     = {mt.getEndTime,     mt.setEndTime}
      funcs.label       = {mt.getLabel,       mt.setLabel}

      return saved_attachData (self, data, funcs)
   end

   -- Initialization.
   local saved_init = assert (mt._init)
   mt._init = function (self)
      self.object:_addEvent (self) -- TODO: REMOVE
      self.object.document:_addEvent (self)
      return saved_init (self)
   end

   -- Finalization.
   local saved_fini = assert (mt._fini)
   mt._fini = function (self)
      self.object:_removeEvent (self) -- TODO: REMOVE
      self.object.document:_removeEvent (self)
      return saved_fini (self)
   end
end
