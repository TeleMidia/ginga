-- Returns qualified id from objId and evtId.
function buildQualifiedId (objId, evtType, evtId)
   if evtType == 'attribution' then
      return objId..'.'..evtId
   elseif evtType == 'presentation' then
      if evtId == '@lambda' then
         return objId..evtId
      else
         return objId..'@'..evtId
      end
   elseif evtType == 'selection' then
      return objId..'<'..evtId..'>'
   else
      error ('bad event type: '..tostring (evtType))
   end
end

-- Event metatable.
do
   local mt = ...

   -- Attaches private data and access functions.
   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, obj, type, id, data, funcs)
      local data  = data or {}
      local funcs = funcs or {}

      -- Private data.
      data._object        = assert (obj)
      data._type          = assert (type)
      data._id            = assert (id)
      data._qualifiedId   = assert (buildQualifiedId (obj.id, type, id))
      data._state         = 'sleeping'
      data._beginTime     = 0
      data._endTime       = nil
      data._label         = nil

      -- Getters & setters.
      funcs.object      = {mt.getObject,      nil}
      funcs.type        = {mt.getType,        nil}
      funcs.id          = {mt.getId,          nil}
      funcs.qualifiedId = {mt.getQualifiedId, nil}
      funcs.state       = {mt.getState,       mt.setState}
      funcs.beginTime   = {mt.getBeginTime,   mt.setBeginTime}
      funcs.endTime     = {mt.getEndTime,     mt.setEndTime}
      funcs.label       = {mt.getLabel,       mt.setLabel}

      return saved_attachData (self, data, funcs)
   end

   -- Initializes private data.
   local saved_init = assert (mt._init)
   mt._init = function (self)
      self.object:_addEvent (self)
      self.object.document:_addEvent (self)
      return saved_init (self)
   end

   -- Finalizes private data.
   local saved_fini = assert (mt._fini)
   mt._fini = function (self)
      self.object:_removeEvent (self)
      self.object.document:_removeEvent (self)
      return saved_fini (self)
   end

   -- Exported functions ---------------------------------------------------

   -- Event::getObject().
   mt.getObject = function (self)
      return assert (mt[self]._object)
   end

   -- Event::getType().
   mt.getType = function (self)
      return assert (mt[self]._type)
   end

   -- Event::getId().
   mt.getId = function (self)
      return assert (mt[self]._id)
   end

   -- Event::getQualifiedId().
   mt.getQualifiedId = function (self)
      return assert (mt[self]._qualifiedId)
   end

   -- Event::getState().
   mt.getState = function (self)
      return assert (mt[self]._state)
   end

   -- Event::setState().
   mt.setState = function (self, state)
      assert (state)
      mt[self]._state = state
   end

   -- Event::getBeginTime().
   mt.getBeginTime = function (self)
      return mt[self]._beginTime
   end

   -- Event::setBeginTime().
   mt.setBeginTime = function (self, time)
      mt[self]._beginTime = time
   end

   -- Event::getEndTime().
   mt.getEndTime = function (self)
      return mt[self]._endTime
   end

   -- Event::setEndTime().
   mt.setEndTime = function (self, time)
      mt[self]._endTime = time
   end

   -- Event::getLabel().
   mt.getLabel = function (self)
      return mt[self]._label
   end

   -- Event::setLabel().
   mt.setLabel = function (self, label)
      mt[self]._label = label
   end
end
