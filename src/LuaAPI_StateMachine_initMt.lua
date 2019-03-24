local assert    = assert
local coroutine = coroutine
local print     = print
local rawget    = rawget
local rawset    = rawset
local type      = type
_ENV = nil

-- Returns qualified id from objId and smId.
local function buildQualifiedId (objId, smType, smId)
   if smType == 'attribution' then
      return objId..'.'..smId
   elseif smType == 'presentation' then
      if smId == '@lambda' then
         return objId..smId
      else
         return objId..'@'..smId
      end
   elseif smType == 'selection' then
      return objId..'<'..smId..'>'
   else
      error ('bad state machine type: '..tostring (smType))
   end
end

-- State machine metatable.
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
      data._type          = assert (type) -- type
      data._id            = assert (id)   -- id
      data._qualifiedId   = assert (buildQualifiedId (obj.id, type, id))
      data._state         = 'sleeping'    -- current state
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

   -- Initializes state machine.
   local saved_init = assert (mt._init)
   mt._init = function (self)
      saved_init (self)
      self.object:_addStateMachine (self)
      self.object.document:_addStateMachine (self)
      --
      -- Default behavior.
      local doc = self.object.document
      self.object:spawn {
         function ()
            if self.type ~= 'presentation'
               or self == self.object.lambda
               or self.label then
               return           -- nothing to do
            end
            -- If we get here, this state machine is an <area>.
            while true do
               doc:_await {statemachine=self.object.lambda,
                          transition='start'}
               if self.beginTime and self.beginTime > 0 then
                  doc:_await {target=self.object, time=self.beginTime,
                              absolute=true}
               end
               self:transition'start'
               if self.endTime and self.endTime > 0 then
                  doc:_parOr {
                     function ()
                        doc:_await {target=self.object, time=self.endTime,
                                    absolute=true}
                     end,
                     function ()
                        doc:_await {statemachine=self.object.lambda,
                                    transition='stop'}
                     end,
                  }
               else
                  doc:_await {statemachine=self.object.lambda,
                              transition='stop'}
               end
               self:transition'stop'
            end
         end
      }
   end

   -- Finalizes state machine.
   local saved_fini = assert (mt._fini)
   mt._fini = function (self)
      self.object:_removeStateMachine (self)
      self.object.document:_removeStateMachine (self)
      saved_fini (self)
   end

   -- Gets a string representation of state machine.
   mt.__tostring = function (self)
      return assert (rawget (mt[self], '_qualifiedId'))
   end

   -- Exported functions ---------------------------------------------------

   mt.getObject = function (self)
      return assert (rawget (mt[self], '_object'))
   end

   mt.getType = function (self)
      return assert (rawget (mt[self], '_type'))
   end

   mt.getId = function (self)
      return assert (rawget (mt[self], '_id'))
   end

   mt.getQualifiedId = function (self)
      return assert (rawget (mt[self], '_qualifiedId'))
   end

   mt.getState = function (self)
      return assert (rawget (mt[self], '_state'))
   end

   mt.setState = function (self, st)
      if st ~= 'occurring' and st ~= 'paused' and st ~= 'sleeping' then
         error ('bad state: '..tostring (st))
      end
      rawset (mt[self], '_state', st)
   end

   mt.getBeginTime = function (self)
      return rawget (mt[self], '_beginTime')
   end

   mt.setBeginTime = function (self, time)
      if type (time) == 'string' then
         time = self.object.document.util.parseTime (time)
      end
      rawset (mt[self], '_beginTime', time)
   end

   mt.getEndTime = function (self)
      return rawget (mt[self], '_endTime')
   end

   mt.setEndTime = function (self, time)
      if type (time) == 'string' then
         time = self.object.document.util.parseTime (time)
      end
      rawset (mt[self], '_endTime', time)
   end

   mt.getLabel = function (self)
      return rawget (mt[self], '_label')
   end

   mt.setLabel = function (self, label)
      rawset (mt[self], '_label', label)
   end

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
      self.object.document:_broadcast {target=self.object,
                                       statemachine=self,
                                       transition=trans}
      return true
   end
end
