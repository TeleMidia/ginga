-- Player metatable.
do
   local mt = ...

   -- Attaches private data and access functions.
   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, media, data, funcs)
      local data  = data or {}
      local funcs = funcs or {}

      -- Private data.
      data._media = assert (media)

      -- Getters & setters.
      funcs.media = {mt.getMedia, nil}
      funcs.eos   = {mt.getEOS,   mt.setEOS}

      return saved_attachData (self, data, funcs)
   end

   -- Initializes private data.
   local saved_init = assert (mt._init)
   mt._init = function (self)
      self.media:_setPlayer (self)
      self.media.document:_addPlayer (self)
      return saved_init (self)
   end

   -- Finalizes private data.
   local saved_fini = assert (mt._fini)
   mt._fini = function (self)
      self.media:_setPlayer (nil)
      self.media.document:_removePlayer (self)
      return saved_fini (self)
   end

   -- Exported functions ---------------------------------------------------

   -- Player::getMedia().
   mt.getMedia = function (self)
      return assert (rawget(mt[self], '_media'))
   end
end
