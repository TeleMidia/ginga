do
   local mt = ...
   local trace = mt._trace
   mt._traceSelf = function (self) return self.qualifiedId end

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
end
