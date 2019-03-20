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
      data._object        = assert (obj)  -- the container object
      data._type          = assert (type) -- event type
      data._id            = assert (id)   -- event id
      data._qualifiedId   = assert (buildQualifiedId (obj.id, type, id))
      data._state         = 'sleeping'    -- event state
      data._beginTime     = 0             -- begin time
      data._endTime       = nil           -- end time
      data._label         = nil           -- label
      data._behaviors = {                 -- behavior data
         start  = {},                     -- behaviors waiting on start
         pause  = {},                     -- behaviors waiting on pause
         resume = {},                     -- behaviors waiting on resume
         stop   = {},                     -- behaviors waiting on stop
         abort  = {},                     -- behaviors waiting on abort
      }

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

   -- Gets the behavior data of object.
   mt._getBehaviorData = function (self)
      return assert (rawget (mt[self], '_behaviors'))
   end

   -- Gets a string representation of event.
   mt.__tostring = function (self)
      return assert (rawget (mt[self], '_qualifiedId'))
   end

   -- Exported functions ---------------------------------------------------

   -- Event::getObject().
   mt.getObject = function (self)
      return assert (rawget (mt[self], '_object'))
   end

   -- Event::getType().
   mt.getType = function (self)
      return assert (rawget (mt[self], '_type'))
   end

   -- Event::getId().
   mt.getId = function (self)
      return assert (rawget (mt[self], '_id'))
   end

   -- Event::getQualifiedId().
   mt.getQualifiedId = function (self)
      return assert (rawget (mt[self], '_qualifiedId'))
   end

   -- Event::getState().
   mt.getState = function (self)
      return assert (rawget (mt[self], '_state'))
   end

   -- Event::setState().
   mt.setState = function (self, st)
      if st ~= 'occurring' and st ~= 'paused' and st ~= 'sleeping' then
         error ('bad state: '..tostring (st))
      end
      rawset (mt[self], '_state', st)
   end

   -- Event::getBeginTime().
   mt.getBeginTime = function (self)
      return rawget (mt[self], '_beginTime')
   end

   -- Event::setBeginTime().
   mt.setBeginTime = function (self, time)
      rawset (mt[self], '_beginTime', time)
   end

   -- Event::getEndTime().
   mt.getEndTime = function (self)
      return rawget (mt[self], '_endTime')
   end

   -- Event::setEndTime().
   mt.setEndTime = function (self, time)
      rawset (mt[self], '_endTime', time)
   end

   -- Event::getLabel().
   mt.getLabel = function (self)
      return rawget (mt[self], '_label')
   end

   -- Event::setLabel().
   mt.setLabel = function (self, label)
      rawset (mt[self], '_label', label)
   end

   -- Event::transition().
   mt.transition = function (self, trans, params)
      local curr = self.state
      local next
      if trans == 'start' then
         if curr == 'occurring' then
            return false
         end
         next = 'occurring'
      elseif trans == 'pause' then
         if curr ~= 'occurring' then
            return false
         end
         next = 'paused'
      elseif trans == 'resume' then
         if curr ~= 'paused' then
            return false
         end
         next = 'occurring'
      elseif trans == 'stop' or trans == 'abort' then
         if curr == 'sleeping' then
            return false
         end
         next = 'sleeping'
      else
         error ('bad transition: '..tostring (trans))
      end

      -- Try to use the behavior interface.
      local before, after = self.object:_getBehavior (self, trans, params)
      if not before then
         before = self.object._beforeTransition
      end
      if not after then
         after = self.object._afterTransition
      end

      -- Initiate transition.
      if not before (self.object, self, trans, params) then
         return false
      end

      -- Update event state.
      self.state = next

      -- Finish transition.
      if not after (self.object, self, trans, params) then
         self.state = curr
         return false
      end

      self.object.document:_awakeBehaviors {event=self, transition=trans}
      return true
   end
end
