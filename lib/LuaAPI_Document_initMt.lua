local _G           = _G
local assert       = assert
local await        = coroutine.yield
local coroutine    = coroutine
local error        = error
local ipairs       = ipairs
local load         = load
local math         = math
local pairs        = pairs
local print        = print
local rawget       = rawget
local rawset       = rawset
local setmetatable = setmetatable
local table        = table
local tostring     = tostring
local type         = type
_ENV = nil

-- Compares two players by zIndex and zOrder.
local function comparePlayers (p1, p2)
   local m1, m2 = p1.media, p2.media
   local z1 = m1:getProperty ('zIndex') or math.mininteger
   local z2 = m2:getProperty ('zIndex') or math.mininteger
   if z1 < z2 then
      return true
   end
   if z1 > z2 then
      return false
   end
   local zo1 = m1:getProperty ('zOrder') or math.mininteger
   local zo2 = m2:getProperty ('zOrder') or math.mininteger
   if zo1 < zo2 then
      return true
   else
      return false
   end
end

-- Dumps object graph rooted at node.
local function dumpGraph (node, prefix)
   local prefix = prefix or ''
   if node:isComposition () then
      print (('%s+ %s::%s'):format (prefix, node.id, node.type))
      local comps = {}
      local atoms = {}
      for _,child in ipairs (node:getChildren ()) do
         if child:isComposition () then
            table.insert (comps, child)
         else
            table.insert (atoms, child)
         end
      end
      for _,atom in ipairs (atoms) do
         dumpGraph (atom, prefix..'  |')
      end
      if #comps == 0 then
         print (prefix)
      else
         for i=1,#comps-1 do
            dumpGraph (comps[i], prefix..'  |')
         end
         dumpGraph (comps[#comps], prefix..'  ')
      end
   else
      print (('%s- %s::%s'):format (prefix, node.id, node.type))
   end
end

-- Parses qualified id.
--
-- Returns the resulting event type, object id, and event id if successful;
-- otherwise returns nil.
--
local function parseQualifiedId (id)
   local tp, o, e

   o, e = id:match ('([%w_-]+)@([%w_-]+)')
   if o and e then
      if e == 'lambda' then
         e = '@lambda'
      end
      tp = 'presentation'
      goto tail
   end

   o, e = id:match ('([%w_-]+)%.([%w._-]+)')
   if o and e then
      tp = 'attribution'
      goto tail
   end

   o, e = id:match ('([%w_-]+)<([%w_-]*)>')
   if o and e then
      tp = 'selection'
   else
      return nil             -- bad format
   end

   ::tail::
   assert (tp)
   assert (o)
   assert (e)
   return tp, o, e
end

-- Document metatable.
do
   local mt = ...

   -- Attaches private data and access functions.
   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, data, funcs)
      local data = data or {}
      local funcs = funcs or {}

      -- Private data.
      data._object    = {}       -- objects indexed by id
      data._event     = {}       -- events indexed by qualified id
      data._parents   = {}       -- table of parents indexed by object
      data._children  = {}       -- table of children indexed by object
      data._players   = {}       -- list of players sorted by zIndex
      data._time      = 0        -- playback time
      data._behaviors = {        -- behavior data
         time = {}               -- behaviors waiting on document time
      }

      local get_data_object = function ()
         return assert (rawget (data, '_object'))
      end
      local get_data_event = function ()
         return assert (rawget (data, '_event'))
      end
      local get_data_parents = function ()
         return rawget (data, '_parents')
      end
      local get_data_children = function ()
         return rawget (data, '_children')
      end
      local get_data_time = function ()
         return assert (rawget (data, '_time'))
      end

      -- Getters & setters.
      funcs.objects  = {mt.getObjects,     nil}
      funcs.root     = {mt.getRoot,        nil}
      funcs.settings = {mt.getSettings,    nil}
      funcs.events   = {mt.getEvents,      nil}
      --
      funcs.object   = {get_data_object,   nil}
      funcs.event    = {get_data_event,    nil}
      funcs.parents  = {get_data_parents,  nil}
      funcs.children = {get_data_children, nil}
      funcs.time     = {get_data_time,     nil}

      return saved_attachData (self, data, funcs)
   end

   -- Initializes private data.
   local saved_init = assert (mt._init)
   mt._init = function (self)
      saved_init (self)
      assert (self:createObject ('context', '__root__'))
      assert (self:createObject ('media', '__settings__'))
   end

   -- Finalizes private data.
   local saved_fini = assert (mt._fini)
   mt._fini = function (self)
      saved_fini (self)
   end

   -- Adds object to document.
   mt._addObject = function (self, obj)
      assert (obj.document == self)
      assert (rawget (mt[self], '_object'))[obj.id] = obj
      assert (rawget (mt[self], '_parents'))[obj] = {}
      if obj:isComposition () then
         assert (rawget (mt[self], '_children'))[obj] = {}
      end
   end

   -- Removes object from document.
   mt._removeObject = function (self, obj)
      assert (obj.document == self)
      for _,parent in ipairs (obj.parents) do
         parent:removeChild (obj)
      end
      if obj:isComposition () then
         for _,child in ipairs (obj.children) do
            obj:removeChild (child)
         end
      end
      assert (rawget (mt[self], '_object'))[obj.id] = nil
   end

   -- Adds event to document.
   mt._addEvent = function (self, evt)
      assert (evt.object.document == self)
      assert (rawget (mt[self], '_event'))[evt.qualifiedId] = evt
   end

   -- Removes event from document.
   mt._removeEvent = function (self, evt)
      assert (evt.object.document == self)
      assert (rawget (mt[self], '_event'))[evt.qualifiedId] = nil
   end

   -- Gets the list of parents of obj.
   mt._getParents = function (self, obj)
      assert (obj.document == self)
      local t = {}
      for k,_ in pairs (self.parents[obj] or {}) do
         table.insert (t, k)
      end
      return t
   end

   -- Gets the list of children of obj.
   mt._getChildren = function (self, obj)
      assert (obj.document == self)
      assert (obj:isComposition ())
      local t = {}
      for k,_ in pairs (assert (self.children[obj])) do
         table.insert (t, k)
      end
      return t
   end

   -- Adds object to parent.
   mt._addChild = function (self, parent, obj)
      assert (parent.document == self)
      assert (obj.document == self)
      assert (parent:isComposition ())
      local tchildren = assert (self.children[parent])
      tchildren[obj] = true
      local tparents = assert (self.parents[obj])
      tparents[parent] = true
   end

   -- Removes object from parent.
   mt._removeChild = function (self, parent, obj)
      assert (parent.document == self)
      assert (obj.document == self)
      local tchildren = assert (self.children[parent])
      tchildren[obj] = nil
      local tparents = assert (self.parents[obj])
      tparents[parent] = nil
   end

   -- Adds player to document.
   mt._addPlayer = function (self, player)
      local media = assert (player.media)
      assert (media.document == self)
      local players = assert (rawget (mt[self], '_players'))
      table.insert (players, player)
      self:_sortPlayers ()
   end

   -- Removes player from document.
   mt._removePlayer = function (self, player)
      local media = assert (player.media)
      assert (media.document == self)
      local players = assert (rawget (mt[self], '_players'))
      for i,v in ipairs (players) do
         if player == v then
            table.remove (players, i)
            self:_sortPlayers ()
            break
         end
      end
   end

   -- Gets the list of players in document sorted by zIndex.
   -- Warning: This list is read-only!
   mt._getPlayers = function (self)
      return assert (rawget (mt[self], '_players'))
   end

   -- Sorts the list of players by zIndex.
   mt._sortPlayers = function (self)
      local players = assert (rawget (mt[self], '_players'))
      table.sort (players, comparePlayers)
   end

   -- Gets player name from URI.
   mt._getPlayerName = function (self, uri)
      local str
      if uri == nil then
         return nil
      end
      local ext = uri:match ('.*%.(.*)$')
      if ext == nil then
         return nil
      end
      if ext == 'lua' then
         return 'PlayerLua'
      else
         return 'PlayerGStreamer'
      end
   end

   -- Dumps document graph.
   mt._dump = function (self)
      local tparents = assert (mt[self].parents)
      local tchildren = assert (mt[self].children)
      local roots = {}
      for _,obj in ipairs (self:getObjects ()) do
         if #obj:getParents () == 0 then
            table.insert (roots, obj)
         end
      end
      for _,root in ipairs (roots) do
         dumpGraph (root)
      end
      return
   end

   -- Gets the behavior data of document.
   mt._getBehaviorData = function (self)
      return assert (rawget (mt[self], '_behaviors'))
   end

   -- Checks condition for any missing data, and updates it if necessary.
   -- If successful, returns the updated condition and the corresponding
   -- behavior data if successful.  Otherwise, returns nil plus an error
   -- message.
   mt._checkBehaviorCondition = function (self, cond)
      if type (cond) ~= 'table' then
         return nil, ('expected condition (got %s)'):format (cond)
      end
      local data
      if cond.time then
         local obj = cond.object
         if not obj then
            obj = self          -- target is document
         elseif obj ~= self then
            if type (obj) == 'string' then
               obj = self:getObject (obj)
            end
            if obj == nil or obj.document ~= self then
               return nil, ('expected object (got %s)'):format (obj)
            end
         end
         cond.object = obj
         data = obj:_getBehaviorData ()
      elseif cond.event and cond.transition then
         local evt = cond.event
         if type (evt) == 'string' then
            evt = self:getEvent (evt)
         end
         if not evt or evt.object.document ~= self then
            return nil, ('expected event (got %s)'):format (evt)
         end
         local trans = cond.transition
         if trans ~= 'start'
            and trans ~= 'pause'
            and trans ~= 'resume'
            and trans ~= 'stop'
            and trans ~= 'abort' then
            return nil, ('expected transition (got %s)'):format (trans)
         end
         cond.event = evt
         cond.object = evt.object
         data = evt:_getBehaviorData ()
      else
         return nil, ('expected condition (got %s)'):format (cond)
      end
      return cond, data
   end

   -- Schedules behavior to execute on condition.
   mt._scheduleBehavior = function (self, co, cond)
      if not cond then
         return                 -- nothing to do
      end
      local cond, data = self:_checkBehaviorCondition (cond)
      assert (cond, data)
      cond.behavior = co
      if cond.time then
         local list = assert (data.time)
         local pos = 1
         while pos <= #list do
            if cond.time < list[pos].time then
               break
            end
         end
         table.insert (list, pos, cond)
      elseif cond.event then
         table.insert (assert (data[cond.transition]), cond)
      else
         error ('should not get here')
      end
   end

   -- Awakes the given behavior.
   mt._awakeBehavior = function (self, co, ...)
      assert (type (co) == 'thread')
      local status, cond = coroutine.resume (co, ...)
      if not status then
         self:_warning ('behavior error: '..tostring (cond))
         cond = nil
      end
      if cond then
         local status, errmsg = self:_checkBehaviorCondition (cond)
         if not status then
            self:_warning ('behavior error: await: '..errmsg)
            cond = nil
         end
      end
      return co, cond
   end

   -- Awakes any behaviors waiting on the given condition.
   mt._awakeBehaviors = function (self, cond)
      local cond, data = self:_checkBehaviorCondition (cond)
      assert (cond, data)
      local schedule = {}
      if cond.time then         -- awake behaviors waiting for time
         local obj = assert (cond.object)
         local list = assert (data.time)
         local time = cond.time
         while #list > 0 and list[1].time <= time do
            local t = table.remove (list, 1)
            local delta = time - t.time -- delta compensation
            local co, cond = self:_awakeBehavior (t.behavior, cond)
            if cond then
               cond, data = self:_checkBehaviorCondition (cond)
               assert (cond, data)
               if cond.time then
                  cond.time = cond.time + cond.object.time - delta
               end
               table.insert (schedule, {co, cond})
            end
         end
      elseif cond.event then    -- awake behaviors waiting for event
         local list = assert (data[cond.transition])
         local awake = {}
         local i = 1
         while i <= #list do
            local match = false
            if cond.params then -- check if parameters match
               error ('not implemented')
            else
               match = true
            end
            if match then
               local t = table.remove (list, i)
               local co, cond = self:_awakeBehavior (t.behavior, cond)
               if cond then
                  cond, data = self:_checkBehaviorCondition (cond)
                  assert (cond, data)
                  table.insert (schedule, {co, cond})
               end
            else
               i = i + 1
            end
         end
      else
         error ('should not get here')
      end
      for _,v in ipairs (schedule) do
         self:_scheduleBehavior (table.unpack (v))
      end
   end

   -- Runs the given function as behavior of object.
   mt._runBehavior = function (self, t, obj, debug)
      if type (t) == 'string' then
         local _await = function (t)
            if type (t) == 'number' then
               return coroutine.yield {object=obj, time=t*1000000}
            else
               return coroutine.yield (t)
            end
         end
         local env = setmetatable ({_D=self, await=_await}, {__index=_G})
         local f, errmsg = load (t, debug, nil, env)
         if not f then
            self:_warning ('behavior error: '..errmsg)
            return
         end
         t = f
      end
      if type (t) == 'function' then
         t = {t}
      end
      assert (type (t) == 'table',
              ('expected behavior (got %s)'):format (t))
      for _,f in ipairs (t) do
         local co = coroutine.create (f)
         self:_scheduleBehavior (self:_awakeBehavior (co, obj or self))
      end
   end

   -- Exported functions ---------------------------------------------------

   -- Document::getObjects().
   mt.getObjects = function (self)
      local t = {}
      for _,obj in pairs (self.object) do
         table.insert (t, obj)
      end
      return t
   end

   -- Document::getObject().
   mt.getObject = function (self, id)
      return self.object[id]
   end

   -- Document::getRoot().
   mt.getRoot = function (self)
      return self.object.__root__
   end

   -- Document::getSettings ().
   mt.getSettings = function (self)
      return self.object.__settings__
   end

   -- Document::createObject().
   mt.createObject = function (self, tp, id)
      return self:_createObject (tp, id)
   end

   -- Document::getEvents().
   mt.getEvents = function (self)
      local t = {}
      for _,evt in pairs (self.event) do
         table.insert (t, evt)
      end
      return t
   end

   -- Document::getEvent().
   mt.getEvent = function (self, id)
      local tp, o, e = parseQualifiedId (id)
      if tp == nil then
         return nil             -- bad format
      end
      local obj = self.object[o]
      if not obj then
         return nil             -- no such object
      end
      return obj:getEvent (tp, e)
   end

   -- Document::createEvent().
   mt.createEvent = function (self, tp, obj, evtId)
      if obj == nil and evtId == nil then
         tp, objId, evtId = parseQualifiedId (tp)
      end
      if type (obj) == 'string' then
         obj = self:getObject (obj)
      end
      assert (obj.document == self)
      return self:_createEvent (tp, obj, evtId)
   end

   -- Document::getTime().
   mt.getTime = function (self)
      return self.time
   end

   -- Document::advanceTime().
   mt.advanceTime = function (self, dt)
      local time = self.time + dt
      rawset (mt[self], '_time', time)
      self:_awakeBehaviors {time=time}
      for _,obj in ipairs (self:getObjects ()) do
         obj:advanceTime (dt)
      end
   end

   -- Runs the given behavior.
   mt.run = function (self, t, debug)
      self:_runBehavior (t, nil, debug)
   end
end
