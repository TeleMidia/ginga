-- Media metatable.
do
   local mt = ...

   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, doc, type, id, data, funcs)
      local data = data or {}
      local funcs = funcs or {}

      -- Private data.

      -- Getters & setters.

      return saved_attachData (self, doc, type, id, data, funcs)
   end

   -- Exported functions ---------------------------------------------------

   -- Object::isComposition().
   mt.isComposition = function (self)
      return false
   end
end
