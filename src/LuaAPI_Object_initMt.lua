local assert       = assert
local coroutine    = coroutine
local ipairs       = ipairs
local print        = print
local rawget       = rawget
local rawset       = rawset
local setmetatable = setmetatable
local table        = table
local tostring     = tostring
local type         = type
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
      data._epoch        = nil           -- doc time when playback started
      data._property     = {}            -- property table
      local _P_mt = {
         __index    = function (_, name)
            return self:getProperty (name)
         end,
         __newindex = function (_, name, value)
            return self:setProperty (name, value)
         end,
      }
      data._P = setmetatable ({}, _P_mt) -- property table access
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
      local get_data_prop = function (...)
         return assert (rawget (data, '_P'))
      end
      --
      -- Access functions.
      funcs.document      = {mt.getDocument, nil}
      funcs.type          = {mt.getType,     nil}
      funcs.id            = {mt.getId,       nil}
      funcs.parents       = {mt.getParents,  nil}
      funcs.statemachines = {mt.getStateMachines,   nil}
      funcs.lambda        = {mt.getLambda,   nil}
      funcs.time          = {mt.getTime,     nil}
      --
      funcs.attribution   = {get_data_attr,  nil}
      funcs.presentation  = {get_data_pres,  nil}
      funcs.selection     = {get_data_seln,  nil}
      funcs.property      = {get_data_prop,  nil}
      --
      return saved_attachData (self, data, funcs)
   end

   -- Initializes object.
   local saved_init = assert (mt._init)
   mt._init = function (self)
      saved_init (self)
      self.document:_addObject (self)
      assert (self:createStateMachine ('presentation', '@lambda'))
      --
      -- Default behavior.
      local await, parOr = self.document._await, self.document._par
      self:spawn {
         function ()            -- start lambda
            while true do
               await {statemachine=self.lambda, transition='start'}
               -- TODO: Start/resume parent if it is not occurring.
               -- TODO: Check if this 'start' is in fact a 'resume'.
               rawset (mt[self], '_epoch', self.document.time)
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

   -- Adds state machine to object.
   mt._addStateMachine = function (self, sm)
      assert (sm)
      assert (rawget (mt[self], '_'..sm.type))[sm.id] = sm
   end

   -- Removes state machine from object.
   mt._removeStateMachine = function (self, sm)
      assert (sm)
      assert (rawget (mt[self], '_'..sm.type))[sm.id] = nil
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

   -- Object::getStateMachines().
   mt.getStateMachines = function (self)
      local t = {}
      for _,tp in ipairs {'attribution', 'presentation', 'selection'} do
         for _,v in pairs (self[tp]) do
            table.insert (t, v)
         end
      end
      return t
   end

   -- Object::getStateMachine().
   mt.getStateMachine = function (self, type, id)
      if type == 'attribution' then
         return self.attribution[id]
      elseif type == 'presentation' then
         return self.presentation[id]
      elseif type == 'selection' then
         return self.selection[id]
      else
         error ('bad state machine type: '..tostring (type))
      end
   end

   -- Object::getLambda().
   mt.getLambda = function (self, type, id)
      return self.presentation['@lambda']
   end

   -- Object::createStateMachine().
   mt.createStateMachine = function (self, type, id)
      return self.document:createStateMachine (type, self, id)
   end

   -- Object::getTime().
   mt.getTime = function (self)
      local epoch = rawget (mt[self], '_epoch')
      if not epoch then
         return nil
      else
         return self.document.time - epoch
      end
   end

   -- Object::getProperty().
   mt.getProperty = function (self, name)
      return assert (rawget (mt[self], '_property'))[name]
   end

   -- Object::setProperty().
   mt.setProperty = function (self, name, value)
      assert (rawget (mt[self], '_property'))[name] = value
      self.document:_awakeBehaviors {target=self, property=name,
                                     value=value}
   end

   -- Spawns behavior.
   mt.spawn = function (self, t, debug)
      self.document:_spawnBehavior (t, self, debug)
   end
end
