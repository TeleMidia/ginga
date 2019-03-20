local assert    = assert
local await     = coroutine.yield
local coroutine = coroutine
local ipairs    = ipairs
local print     = print
local rawget    = rawget
local rawset    = rawset
local table     = table
local tostring  = tostring
local type      = type
_ENV = nil

-- Object metatable.
do
   local mt = ...

   -- Attaches private data and access functions.
   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, doc, type, id, data, funcs)
      local data = data or {}
      local funcs = funcs or {}
      --
      -- Private data.
      data._document     = assert (doc)  -- the container document
      data._type         = assert (type) -- object type
      data._id           = assert (id)   -- object id
      data._attribution  = {}            -- attribution evts indexed by id
      data._presentation = {}            -- presentation evts indexed by id
      data._selection    = {}            -- selection evts indexed by id
      data._time         = nil           -- playback time
      data._property     = {}            -- property table
      data._behaviors = {                -- behavior data
         time = {}                       -- behaviors waiting on object time
      }
      --
      local get_data_attr = function ()
         return assert (rawget (data, '_attribution'))
      end
      local get_data_pres = function ()
         return assert (rawget (data, '_presentation'))
      end
      local get_data_seln = function (...)
         return assert (rawget (data, '_selection'))
      end
      --
      -- Getters & setters.
      funcs.document     = {mt.getDocument, nil}
      funcs.type         = {mt.getType,     nil}
      funcs.id           = {mt.getId,       nil}
      funcs.parents      = {mt.getParents,  nil}
      funcs.events       = {mt.getEvents,   nil}
      funcs.lambda       = {mt.getLambda,   nil}
      funcs.time         = {mt.getTime,     nil}
      --
      funcs.attribution  = {get_data_attr,  nil}
      funcs.presentation = {get_data_pres,  nil}
      funcs.selection    = {get_data_seln,  nil}
      --
      return saved_attachData (self, data, funcs)
   end

   -- Initializes object.
   local saved_init = assert (mt._init)
   mt._init = function (self)
      saved_init (self)
      self.document:_addObject (self)
      assert (self:createEvent ('presentation', '@lambda'))
      --
      -- Default behavior.
      self:run {
         function ()            -- start lambda
            while true do
               await {event=self.lambda, transition = 'start'}
               print ('object', self.id, 'lambda start')
               -- TODO: Start/resume parent if it is not occurring.
               -- TODO: Check if this 'start' is in fact a 'resume'.
               self:setTime (0)
            end
         end
      }
   end

   -- Finalizes object.
   local saved_fini = assert (mt._fini)
   mt._fini = function (self)
      self.document:_removeObject (self)
      saved_fini (self)
   end

   -- Adds event to object.
   mt._addEvent = function (self, evt)
      assert (evt)
      assert (rawget (mt[self], '_'..evt.type))[evt.id] = evt
   end

   -- Removes event from object.
   mt._removeEvent = function (self, evt)
      assert (evt)
      assert (rawget (mt[self], '_'..evt.type))[evt.id] = nil
   end

   -- Gets the behavior data of object.
   mt._getBehaviorData = function (self)
      return assert (rawget (mt[self], '_behaviors'))
   end

   -- Gets a string representation of object.
   mt.__tostring = function (self)
      return assert (rawget (mt[self], '_id'))
   end

   -- Exported functions ---------------------------------------------------

   -- Object::isComposition().
   mt.isComposition = function (self)
      error ('should be implemented by subclasses')
   end

   -- Object::getDocument().
   mt.getDocument = function (self)
      return assert (rawget (mt[self], '_document'))
   end

   -- Object::getType().
   mt.getType = function (self)
      return assert (rawget (mt[self], '_type'))
   end

   -- Object::getId().
   mt.getId = function (self)
      return assert (rawget (mt[self], '_id'))
   end

   -- Object::getParents().
   mt.getParents = function (self)
      return self.document:_getParents (self)
   end

   -- Object::getEvents().
   mt.getEvents = function (self)
      local t = {}
      for _,tp in ipairs {'attribution', 'presentation', 'selection'} do
         for _,v in pairs (self[tp]) do
            table.insert (t, v)
         end
      end
      return t
   end

   -- Object::getEvent().
   mt.getEvent = function (self, type, id)
      if type == 'attribution' then
         return self.attribution[id]
      elseif type == 'presentation' then
         return self.presentation[id]
      elseif type == 'selection' then
         return self.selection[id]
      else
         error ('bad event type: '..tostring (type))
      end
   end

   -- Object::getLambda().
   mt.getLambda = function (self, type, id)
      return self.presentation['@lambda']
   end

   -- Object::createEvent().
   mt.createEvent = function (self, type, id)
      return self.document:createEvent (type, self, id)
   end

   -- Object::getTime().
   mt.getTime = function (self)
      return rawget (mt[self], '_time')
   end

   -- Object:setTime().
   mt.setTime = function (self, time)
      rawset (mt[self], '_time', time)
   end

   -- Object:advanceTime().
   mt.advanceTime = function (self, dt)
      if self.lambda.state ~= 'occurring' then
         return                 -- nothing to do
      end
      local time = (self.time or 0) + dt
      self:setTime (time)
      self.document:_awakeBehaviors {object=self, time=time}
   end

   -- Object::getProperty().
   mt.getProperty = function (self, name)
      return assert (rawget (mt[self], '_property'))[name]
   end

   -- Object::setProperty().
   mt.setProperty = function (self, name, value)
      assert (rawget (mt[self], '_property'))[name] = value
   end

   -- Runs the given behavior.
   mt.run = function (self, t, debug)
      self.document:_runBehavior (t, self, debug)
   end
end
