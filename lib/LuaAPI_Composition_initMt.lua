do
   local mt = ...

   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, doc, type, id, data, funcs)
      local data = data or {}
      local funcs = funcs or {}

      -- Private data.

      -- Getters & setters.
      funcs.children      = {mt.getChildren,       nil}
      funcs.child         = {mt.getChild,          nil}

      return saved_attachData (self, doc, type, id, data, funcs)
   end

   -- Exported functions ---------------------------------------------------

   -- Object::isComposition().
   mt.isComposition = function (self)
      return true
   end

   -- Composition::getChildren().
   mt.getChildren = function (self)
      return self.document:_getChildren (self)
   end

   -- Composition::getChild().
   mt.getChild = function (self, id)
      for _,v in ipairs (self:getChildren ()) do
         if v.id == id then
            return v
         end
      end
      return nil
   end

   -- Composition::addChild().
   mt.addChild = function (self, obj)
      self.document:_addChild (self, obj)
   end

   -- Composition::removeChild().
   mt.removeChild = function (self, obj)
      self.document:_removeChild (self, obj)
   end
end
