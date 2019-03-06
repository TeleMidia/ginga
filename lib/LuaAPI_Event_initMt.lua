do
   local mt = ...
   local trace = mt._trace

   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self)
      local data  = {}
      local funcs = {
         object      = {mt.getObject,      nil},
         type        = {mt.getType,        nil},
         id          = {mt.getId,          nil},
         qualifiedId = {mt.getQualifiedId, nil},
         state       = {mt.getState,       mt.setState},
         beginTime   = {mt.getBeginTime,   mt.setBeginTime},
         endTime     = {mt.getEndTime,     mt.setEndTime},
         label       = {mt.getLabel,       mt.setLabel},
      }
      return saved_attachData (self, data, funcs)
   end
end
