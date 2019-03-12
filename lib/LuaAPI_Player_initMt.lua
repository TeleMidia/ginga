-- Player metatable.
do
   local mt = ...

   -- Attaches private data and access functions.
   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, media, data, funcs)
      local data  = data or {}
      local funcs = funcs or {}

      -- Private data.
      -- none

      -- Getters & setters.
      -- none

      return saved_attachData (self, data, funcs)
   end
end
