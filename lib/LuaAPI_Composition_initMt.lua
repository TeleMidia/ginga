do
   local mt = ...
   local trace = mt._trace
   mt._traceOff.getChildren = true
   -- mt._traceOff.getChild    = true
   -- mt._traceOff.addChild    = true
   -- mt._traceOff.removeChild = true

   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, doc, type, id, data, funcs)
      local data = data or {}
      local funcs = funcs or {}

      -- Private data.
      data._composition = true

      local get_data_composition = function ()
         return assert (data._composition)
      end

      -- Getters & setters.
      funcs.children      = {mt.getChildren,       nil}
      funcs.child         = {mt.getChild,          nil}
      --
      funcs.composition   = {get_data_composition, nil}

      return saved_attachData (self, doc, type, id, data, funcs)
   end

   -- Composition::getChildren().
   mt.getChildren = function (self)
      trace (self, 'getChildren')
      return self.document:_getChildren (self)
   end

   -- Composition::getChild().
   mt.getChild = function (self, id)
      trace (self, 'getChild', id)
      for _,v in ipairs (self:getChildren ()) do
         if v.id == id then
            return v
         end
      end
      return nil
   end

   -- Composition::addChild().
   mt.addChild = function (self, obj)
      trace (self, 'addChild', obj)
      self.document:_addChild (self, obj)
   end

   -- Composition::removeChild().
   mt.removeChild = function (self, obj)
      trace (self, 'removeChild', obj)
      self.document:_removeChild (self, obj)
   end
end
