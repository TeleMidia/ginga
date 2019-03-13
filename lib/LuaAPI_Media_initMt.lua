-- Media metatable.
do
   local mt = ...

   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, doc, type, id, data, funcs)
      local data = data or {}
      local funcs = funcs or {}

      -- Private data.
      data._player = nil

      -- Getters & setters.
      funcs.player = {mt._getPlayer, nil}

      return saved_attachData (self, doc, type, id, data, funcs)
   end

   -- Gets the player of media object.
   mt._getPlayer = function (self)
      return rawget (mt[self], '_player')
   end

   -- Sets the player of media object.
   mt._setPlayer = function (self, player)
      rawset (mt[self], '_player', player)
   end

   -- Behaviors ------------------------------------------------------------

   mt._behavior.before.lambda.start = function (self, evt, trans, params)
      assert (evt.object == self)
      assert (evt.id == '@lambda')

      -- Start/resume parent context if it is not occurring.
      local parent = self.parent
      if parent and parent.lambda.state ~= 'occurring' then
         error ('TODO: Start parent')
      end

      -- Check if this start is in fact a resume.
      if self.state == 'paused' then
         error ('TODO: Resume object')
      end

      -- Create the underlying player.
      assert (self.player == nil)
      local type, player = self.document
         :_createPlayer (self, self.property.uri, self.property.type)
      assert (player)
      self:setProperty ('type', assert (type))

      -- Start the underlying player.
      if not player:start () then
         return false
      end

      return true
   end

   -- mt._behavior.after.lambda.start = function (self, evt, trans, params)
   --    assert (evt.object == self)
   --    assert (evt.id == '@lambda')
   --    return true
   -- end

   -- Exported functions ---------------------------------------------------

   -- Object::isComposition().
   mt.isComposition = function (self)
      return false
   end

   -- Object::setProperty().
   local saved_setProperty = mt.setProperty
   mt.setProperty = function (self, name, value)
      if name == "zIndex" or name == "zOrder" then
         local value = tonumber (value)
         if value == nil then
            return false
         end
         assert (saved_setProperty (self, name, value))
         self.document:_sortPlayers ()
      else
         return saved_setProperty (self, name, value)
      end
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
