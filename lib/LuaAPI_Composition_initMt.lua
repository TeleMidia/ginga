do
   local mt = ...
   local trace = mt._trace

   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, data, funcs)
      local data = data or {}
      local funcs = funcs or {}

      -- Getters & setters.
      funcs.children = {mt.getChildren, nil}

      return saved_attachData (self, data, funcs)
   end

   -- Implementation of Composition::getChildren().
   mt.getChildren = function (self)
      trace ('getChildren (%s)', self.id)
      error ('not implemented')
   end
end
