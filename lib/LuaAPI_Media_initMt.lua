local function parseNumber (str)
   local n = tonumber (str)
   if n then
      return n
   end
   if str:sub (-2) == 'px' then
      return tonumber (str:sub (1,-3))
   end
   if str:sub (-1) == '%' then
      n = tonumber (str:sub (1,-2))
      if n then
         return n / 100.
      end
   end
   return nil
end

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

   -- Updates the dimensions of the drawing area of the player of media
   -- object.
   mt._updatePlayerRect = function (self)
      if not self.player then
         return                 -- nothing to do
      end
      local swidth  = assert (self.document.settings:getProperty ('width'))
      local sheight = assert (self.document.settings:getProperty ('height'))
      --
      -- Get width in pixels.
      local width = self:getProperty ('width') or 1.0
      if not self.document._isinteger (width) then
         width = math.floor (swidth * width)
      end
      --
      -- Get height in pixels.
      local height = self:getProperty ('height') or 1.0
      if not self.document._isinteger (height) then
         height = math.floor (sheight * height)
      end
      --
      -- Get x in pixels.
      local x = self:getProperty ('left')
      if not x then
         x = self:getProperty ('right')
         if not x then
            x = 0               -- default value
         else
            if not self.document._isinteger (x) then
               x = math.floor (swidth * x)
            end
            x = swidth - width - x
         end
      elseif not self.document._isinteger (x) then
         x = math.floor (swidth * x)
      end
      --
      -- Get y in pixels.
      local y = self:getProperty ('top')
      if not y then
         y = self:getProperty ('bottom')
         if not y then
            y = 0               -- default value
         else
            if not self.document._isinteger (y) then
               y = math.floor (sheight * y)
            end
            y = sheight - height - y
         end
      elseif not self.document._isinteger (y) then
         y = math.floor (sheight * y)
      end
      self.player:setRect (x, y, width, height)
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

      -- Create player.
      assert (self.player == nil)
      local uri = self:getProperty ('uri')
      local player = self.document:_createPlayer (self, uri)
      assert (player)
      assert (self.player == player)

      -- Initialize player.
      player:setURI (uri)
      self:_updatePlayerRect ()

      -- Start player.
      player:start ()

      return true
   end

   mt._behavior.after.lambda.start = function (self, evt, trans, params)
      assert (evt.object == self)
      assert (evt.id == '@lambda')
      self:setTime (0)
      return true
   end

   -- Exported functions ---------------------------------------------------

   -- Object::isComposition().
   mt.isComposition = function (self)
      return false
   end

   -- Object::getProperty() and Object::setProperty().
   local saved_getProperty = mt.getProperty
   local saved_setProperty = mt.setProperty

   local _setProperty_width_height = function (self, name, value)
      if type (value) == 'string' then
         value = parseNumber (value)
      end
      if type (value) ~= 'number' then
         return                 -- bad value, do nothing
      end
      value = math.abs (value)
      saved_setProperty (self, name, value)
      if self ~= self.document.settings then
         self:_updatePlayerRect ()
      else
         for _,v in ipairs (assert (self.document.objects)) do
            if v.type == 'media' then
               v:_updatePlayerRect ()
            end
         end
      end
   end

   local _setProperty_tblr = function (self, name, value)
      if type (value) == 'string' then
         value = parseNumber (value)
      end
      if type (value) ~= 'number' then
         return                 -- bad value, do nothing
      end
      if name == 'top' then
         saved_setProperty (self, 'bottom', nil)
      elseif name == 'bottom' then
         saved_setProperty (self, 'top', nil)
      elseif name == 'left' then
         saved_setProperty (self, 'right', nil)
      elseif name == 'right' then
         saved_setProperty (self, 'left', nil)
      else
         error ('should not get here')
      end
      saved_setProperty (self, name, value)
      self:_updatePlayerRect ()
   end

   local _getProperty_bounds = function (self, name)
      error ('TODO')
   end

   local _setProperty_bounds_re
      ='^%s*([%w%.]+%%?)%s*,%s*([%w%.]+%%?)%s*,'
      ..'%s*([%w%.]+%%?)%s*,%s*([%w%.]+%%?)%s*$'

   local _setProperty_bounds = function (self, name, value)
      local top, left, width, height
      if type (value) == 'table' then
         top, left, width, height = table.unpack (value)
      elseif type (value) == 'string' then
         top, left, width, height = value:match (_setProperty_bounds_re)
      end
      if not top then
         return                 -- bad value, nothing to do
      end
      self:setProperty ('top', top)
      self:setProperty ('left', left)
      self:setProperty ('width', width)
      self:setProperty ('height', height)
   end

   local _setProperty_z = function (self, name, value)
      local value = tonumber (value)
      if value == nil then
         return                 -- bad value, do nothing
      end
      assert (saved_setProperty (self, name, value))
      self.document:_sortPlayers ()
   end

   local _knownProperties = {
      width  = {nil, _setProperty_width_height},
      height = {nil, _setProperty_width_height},
      top    = {nil, _setProperty_tblr},
      bottom = {nil, _setProperty_tblr},
      left   = {nil, _setProperty_tblr},
      right  = {nil, _setProperty_tblr},
      bounds = {nil, _setProperty_bounds},
      zIndex = {nil, _setProperty_z},
      zOrder = {nil, _setProperty_z},
   }

   mt.getProperty = function (self, name)
      local t = _knownProperties[name]
      if t and t[1] then
         return t[1] (self, name)
      else
         return saved_getProperty (self, name)
      end
   end

   mt.setProperty = function (self, name, value)
      local t = _knownProperties[name]
      if t and t[2] then
         return t[2] (self, name, value)
      else
         return saved_setProperty (self, name, value)
      end
   end

   -- Media::isFocused().
   mt.isFocused = function (self)
      if self.lambda.state ~= 'occurring' then
         return false
      end
      local focus = self.document.settings
         :getProperty ('service.currentFocus')
      if not focus then
         return false
      end
      local me = self:getProperty ('focusIndex')
      if not me then
         return false
      end
      return focus == me
   end
end
