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

   -- Media::isFocused().
   mt.isFocused = function (self)
      if self.lambda.state ~= 'occurring' then
         return false
      end
      local focus = self.document.settings.property['service.currentFocus']
      if not focus then
         return false
      end
      local me = self.property.focusIndex
      if not me then
         return false
      end
      return focus == me
   end
end
