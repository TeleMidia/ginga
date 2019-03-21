local assert    = assert
local coroutine = coroutine
local print     = print
local rawget    = rawget
local rawset    = rawset
_ENV = nil

-- Returns qualified id from objId and evtId.
local function buildQualifiedId (objId, evtType, evtId)
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
      --
      -- Private data.
      data._object        = assert (obj)  -- the container object
      data._type          = assert (type) -- event type
      data._id            = assert (id)   -- event id
      data._qualifiedId   = assert (buildQualifiedId (obj.id, type, id))
      data._state         = 'sleeping'    -- event state
      data._beginTime     = 0             -- begin time
      data._endTime       = nil           -- end time
      data._label         = nil           -- label
      --
      -- Getters & setters.
      funcs.object      = {mt.getObject,      nil}
      funcs.type        = {mt.getType,        nil}
      funcs.id          = {mt.getId,          nil}
      funcs.qualifiedId = {mt.getQualifiedId, nil}
      funcs.state       = {mt.getState,       mt.setState}
      funcs.beginTime   = {mt.getBeginTime,   mt.setBeginTime}
      funcs.endTime     = {mt.getEndTime,     mt.setEndTime}
      funcs.label       = {mt.getLabel,       mt.setLabel}
      --
      return saved_attachData (self, data, funcs)
   end

   -- Initializes event.
   local saved_init = assert (mt._init)
   mt._init = function (self)
      saved_init (self)
      self.object:_addEvent (self)
      self.object.document:_addEvent (self)
      --
      -- Default behavior.
      local await = self.object.document._await
      local parOr = self.object.document._par
      self.object:spawn {
         function ()
            if self.type ~= 'presentation'
               or self == self.object.lambda
               or self.label then
               return           -- nothing to do
            end
            -- If we get here, this event is an <area>.
            while true do
               await {event=self.object.lambda, transition='start'}
               if self.beginTime and self.beginTime > 0 then
                  await {object=self.object, time=self.beginTime,
                         absolute=true}
               end
               print ('event', self.qualifiedId, 'start', self.object.time)
               self:transition ('start')
               if self.endTime and self.endTime > 0 then
                  parOr {
                     function ()
                        await {object=self.object, time=self.endTime,
                               absolute=true}
                     end,
                     function ()
                        await {event=self.object.lambda, transition='stop'}
                     end,
                  }
               else
                  await {event=self.object.lambda, transition='stop'}
               end
               print ('event', self.qualifiedId, 'stop', self.object.time)
               self:transition ('stop')
            end
         end
      }
   end

   -- Finalizes event.
   local saved_fini = assert (mt._fini)
   mt._fini = function (self)
      self.object:_removeEvent (self)
      self.object.document:_removeEvent (self)
      saved_fini (self)
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
      self.state = next
      self.object.document:_awakeBehaviors {event=self, transition=trans}
      return true
   end
end
