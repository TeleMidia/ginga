local _G           = _G
local assert       = assert
local coroutine    = coroutine
local error        = error
local ipairs       = ipairs
local load         = load
local math         = math
local math         = math
local os           = os
local pairs        = pairs
local print        = print
local rawget       = rawget
local rawset       = rawset
local setmetatable = setmetatable
local table        = table
local tonumber     = tonumber
local tostring     = tostring
local type         = type
_ENV = nil

-- Compares two players by zIndex and zOrder.  Returns true if p1 has a
-- lower z than p2, otherwise returns false.
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

-- Tests whether two conditions match.
local function matchConditions (triggered, awaited)
   assert (type (triggered) == 'table')
   assert (type (awaited) == 'table')
   if awaited.time then         -- time
      if not triggered.time then
         return false
      end
      if triggered.target ~= awaited.target then
         return false
      end
      if triggered.time < awaited.time then
         return false
      end
      return true
   end
   local t = triggered          -- generic match
   for k,v in pairs (awaited) do
      if not t[k] or t[k] ~= v then
         return false
      end
   end
   return true
end

-- Traverses "par" tree triggering the conditions satisfied by cond.
-- Returns the resulting "par" tree and a flag indicating whether any
-- condition was triggered.
local function parsePar (cond, par)
   assert (par[0] == 'par')
   local flag = false
   local result = {[0]='par'}
   for i,v in ipairs (par) do
      local status
      if v[0] == 'par' then
         result[i], status = parsePar (cond, v)
      else
         status = matchConditions (cond, v)
         if status then
            result[i] = cond    -- ith trail should be awaken
         else
            result[i] = false   -- no-op
         end
      end
      if status then
         flag = true            -- some trail should be awaken
      end
   end
   return result, flag
end

-- Parses qualified id.  If successful, returns the resulting state machine
-- type, object id, and state machine id.  Otherwise, returns nil.
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
   local dump = mt._dump
   --
   -- Attaches private data and access functions.
   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, data, funcs)
      local data = data or {}
      local funcs = funcs or {}
      --
      -- Private data.
      data._object    = {}       -- objects indexed by id
      data._sm        = {}       -- state machines indexed by qualified id
      data._parents   = {}       -- table of parents indexed by object
      data._children  = {}       -- table of children indexed by object
      data._players   = {}       -- list of players sorted by zIndex
      data._time      = 0        -- playback time
      data._accum     = 0        -- accumulated "real" time
      data._waitlist  = {}       -- list of behaviors waiting for conditions
      --
      local get_data_object = function ()
         return assert (rawget (data, '_object'))
      end
      local get_data_sm = function ()
         return assert (rawget (data, '_sm'))
      end
      local get_data_parents = function ()
         return rawget (data, '_parents')
      end
      local get_data_children = function ()
         return rawget (data, '_children')
      end
      --
      -- Access functions.
      funcs.objects  = {mt.getObjects,       nil}
      funcs.root     = {mt.getRoot,          nil}
      funcs.settings = {mt.getSettings,      nil}
      funcs.machines = {mt.getStateMachines, nil}
      funcs.time     = {mt.getTime,          nil}
      --
      funcs.object   = {get_data_object,     nil}
      funcs.sm       = {get_data_sm,         nil}
      funcs.parents  = {get_data_parents,    nil}
      funcs.children = {get_data_children,   nil}
      --
      return saved_attachData (self, data, funcs)
   end

   -- Initializes document.
   local saved_init = assert (mt._init)
   mt._init = function (self)
      saved_init (self)
      assert (self:createObject ('context', '__root__'))
      assert (self:createObject ('media', '__settings__'))
   end

   -- Finalizes document.
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

   -- Adds state machine to document.
   mt._addStateMachine = function (self, sm)
      assert (sm.object.document == self)
      assert (rawget (mt[self], '_sm'))[sm.qualifiedId] = sm
   end

   -- Removes state machine from document.
   mt._removeStateMachine = function (self, sm)
      assert (sm.object.document == self)
      assert (rawget (mt[self], '_sm'))[sm.qualifiedId] = nil
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
   mt._dumpGraph = function (self)
      local tparents = assert (mt[self].parents)
      local tchildren = assert (mt[self].children)
      local roots = {}
      for _,obj in ipairs (self.objects) do
         if #obj:getParents () == 0 then
            table.insert (roots, obj)
         end
      end
      for _,root in ipairs (roots) do
         dumpGraph (root)
      end
   end

   -- Schedules behavior to execute on condition.
   mt._scheduleBehavior = function (self, co, cond)
      if not cond then
         return
      end
      assert (cond[0] == 'par')
      cond.behavior = co
      table.insert (assert (rawget (mt[self], '_waitlist')), cond)
   end

   -- Awakes the given behavior.
   mt._awakeBehavior = function (self, co, ...)
      assert (type (co) == 'thread')
      local status, cond = coroutine.resume (co, ...)
      if not status then
         self:_error ('behavior error: '..tostring (cond))
         cond = nil
      end
      return co, cond
   end

   -- Awakes any behaviors waiting on the given condition.
   mt._awakeBehaviors = function (self, cond)
      local schedule = {}
      local waitlist = assert (rawget (mt[self], '_waitlist'))
      local i = 1
      while i <= #waitlist do
         local par = assert (waitlist[i])
         assert (par[0] == 'par')
         local result, flag = parsePar (cond, par)
         if flag then
            table.remove (waitlist, i)
            local co, cond = self:_awakeBehavior (par.behavior, result)
            if cond then
               table.insert (schedule, {co, cond})
            end
         else
            i = i + 1        -- nothing to do
         end
      end
      for _,v in ipairs (schedule) do
         self:_scheduleBehavior (table.unpack (v))
      end
   end

   -- Spawns the given function as a new behavior of object.
   mt._spawnBehavior = function (self, t, obj, debug)
      if type (t) == 'string' then
         local _await = function (t)
            if type (t) == 'number' then
               return mt._await {target=(obj or self), time=t*1000}
            else
               return mt._await (t)
            end
         end
         local env = {_D=self, await=_await, parOr=mt._par}
         setmetatable (env, {__index=_G})
         local f, errmsg = load (t, debug, nil, env)
         if not f then
            self:_error ('behavior error: '..errmsg)
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

   -- The "await" operator to be used inside behaviors.
   mt._await = function (t)
      assert (type (t) == 'table')
      if t.time then
         if not t.absolute then
            t.time = t.time + (t.target.time or 0)
         end
      end
      local res
      repeat
         res = assert (coroutine.yield {[0]='par', t})
         assert (res[0] == 'par')
         res = res[1]
      until res
      return res
   end

   -- The "par" operator to be used inside behaviors.
   mt._par = function (t)
      assert (type (t) == 'table')
      if #t == 0 then
         return                 -- no trails, nothing to do
      end
      --
      -- Wrap trail functions into coroutines.
      for i,f in ipairs (t) do
         assert (type (f) == 'function')
         t[i] = coroutine.create (f)
      end
      local trails = t
      --
      -- Bootstrap trails.
      local conds = {[0]='par'}
      for i,co in ipairs (trails) do
         local status, cond = coroutine.resume (co)
         assert (status, cond)
         if cond == nil then
            return              -- ith trail terminated
         end
         conds[i] = cond
      end
      --
      -- Awake trails.
      while true do
         local result = coroutine.yield (conds)
         local nextconds = {[0]='par'}
         for i,res in ipairs (result) do
            if res == false then
               nextconds[i] = assert (conds[i])
            elseif type (res) == 'table' then
               --
               -- Awake i-th trail.
               local status, cond = coroutine.resume (trails[i], res)
               assert (status, cond)
               if cond == nil then
                  return        -- ith trail terminated
               end
               nextconds[i] = cond
            else
               error ('should not get here')
            end
         end
         conds = nextconds
      end
   end

   -- Exported functions ---------------------------------------------------

   mt.getObjects = function (self)
      local t = {}
      for _,obj in pairs (self.object) do
         table.insert (t, obj)
      end
      return t
   end

   mt.getObject = function (self, id)
      return self.object[id]
   end

   mt.getRoot = function (self)
      return self.object.__root__
   end

   mt.getSettings = function (self)
      return self.object.__settings__
   end

   mt.createObject = function (self, tp, id)
      return self:_createObject (tp, id)
   end

   mt.getStateMachines = function (self)
      local t = {}
      for _,sm in pairs (self.sm) do
         table.insert (t, sm)
      end
      return t
   end

   mt.getStateMachine = function (self, id)
      local tp, o, e = parseQualifiedId (id)
      if tp == nil then
         return nil             -- bad format
      end
      local obj = self.object[o]
      if not obj then
         return nil             -- no such object
      end
      return obj:getStateMachine (tp, e)
   end

   mt.createStateMachine = function (self, tp, obj, smId)
      if obj == nil and smId == nil then
         tp, objId, smId = parseQualifiedId (tp)
      end
      if type (obj) == 'string' then
         obj = self:getObject (obj)
      end
      assert (obj.document == self)
      return self:_createStateMachine (tp, obj, smId)
   end

   mt.getTime = function (self)
      return rawget (mt[self], '_time')
   end

   local quantum = 1000         -- us
   local function _advanceObjectTime (self, obj)
      self:_awakeBehaviors {target=obj, time=obj.time}
      if obj:isComposition () then
         for _,child in ipairs (obj.children) do
            _advanceObjectTime (self, child)
         end
      end
   end

   local function _advanceDocumentTime (self, time)
      self:_awakeBehaviors {target=self, time=time}
      local tparents = assert (mt[self].parents)
      local tchildren = assert (mt[self].children)
      local roots = {}
      for _,obj in ipairs (self.objects) do
         if #obj:getParents () == 0
            and obj ~= self.root
            and obj ~= self.settings then
            table.insert (roots, obj)
         end
      end
      _advanceObjectTime (self, self.root)
      _advanceObjectTime (self, self.settings)
      for _,root in ipairs (roots) do
         _advanceObjectTime (self, root)
      end
   end

   mt.advanceTime = function (self, us)
      if us == 0 then           -- bootstrap (empty) tick
         _advanceDocumentTime (self, 0)
      else
         local accum = assert (rawget (mt[self], '_accum'))
         accum = accum + us
         if accum > 0 and accum < quantum then
            return
         end
         local n, delta = math.modf (accum/quantum)
         delta = math.modf (delta)
         rawset (mt[self], '_accum', delta)
         local ms = self.time
         for i=1,n do           -- generate n ticks
            rawset (mt[self], '_time', ms + i)
            _advanceDocumentTime (self, ms + i)
         end
      end
   end

   mt.sendKey = function (self, key, press)
      local str = tostring (key)
      if not key then
         return                 -- nothing to do
      end
      local t = {target=self, key=key}
      if press then
         t.type = 'press'
      else
         t.type = 'release'
      end
      self:_awakeBehaviors (t)
   end

   mt.spawn = function (self, t, debug)
      self:_spawnBehavior (t, nil, debug)
   end

   -- Utility functions ----------------------------------------------------
   mt.util = {isinteger=mt._isinteger}

   -- Parses a dimension value (e.g, '100%', '10px').  Returns a number if
   -- the value is relative.  Otherwise, if value is absolute, returns an
   -- integer.
   mt.util.parseDimension = function (str)
      local n = tonumber (str)
      if n then
         return n
      end
      if str:sub (-2) == 'px' then
         return math.tointeger (math.floor (tonumber (str:sub (1,-3))))
      end
      if str:sub (-1) == '%' then
         n = tonumber (str:sub (1,-2))
         if n then
            return n / 100.
         end
      end
      return nil
   end

   -- Parses a position value.
   mt.util.parsePosition = mt.util.parseDimension

   -- Parses a time value (e.g., 10ms, 10s).
   mt.util.parseTime = function (str)
      local n = tonumber (str)
      if n then
         return n
      end
      if str:sub (-2) == 'ms' then
         return math.modf (tonumber (str:sub (1,-3)))
      end
      if str:sub (-1) == 's' then
         local s = math.modf (tonumber (str:sub (1,-2)))
         if s then
            return s * 1000     -- ms
         end
      end
      return nil
   end
end
