---
-- The Behavior class.
-- @classmod Behavior

local assert       = assert
local coroutine    = coroutine
local debug        = debug
local error        = error
local getmetatable = getmetatable
local ipairs       = ipairs
local pairs        = pairs
local print        = print
local setmetatable = setmetatable
local table        = table
local tostring     = tostring
local type         = type

local _ENV = nil
local behavior = {}
do
   behavior.__index = behavior
end

local function args2str (...)
   local res = {}
   for _,v in ipairs {...} do
      table.insert (res, tostring (v))
   end
   return table.concat (res, ',')
end

local function list2str (t)
   return '{'..args2str (table.unpack (t))..'}'
end

local function tab2str (t)
   local res = {}
   for k,v in pairs (t) do
      if type (k) == 'string' then
         k = "'"..k.."'"
      else
         k = tostring (k)
      end
      if type (v) == 'string' then
         v = "'"..v.."'"
      else
         v = tostring (v)
      end
      table.insert (res, ('[%s]=%s'):format (k,v))
   end
   return '{'..table.concat (res, ',')..'}'
end

-- Default error handler.
local function bhvError (bhv, message)
   error (tostring (bhv)..': '..message, 2)
end

-- Auxiliary function used by behavior._dump().
local function bhvDump (prefix, node, indent, highlight)
   local me = ('  '):rep (indent or 0)..tostring (node)
   if node == highlight then
      me = me..'*'
   end
   if node:_isRoot () then
      me = me..list2str (node.stack)
   end
   print (prefix..me)
   for _,child in ipairs (node.children) do
      bhvDump (prefix, child, indent + 1, highlight)
   end
end

-- Creates a new behavior.
local function bhvNew (f, name, parent)
   local co = assert (coroutine.create (f))
   local t = {
      co       = co,                    -- the underlying coroutine
      name     = name or tostring (co), -- behavior name
      parent   = parent,                -- parent behavior
      children = {},                    -- list of child behaviors
      awaiting = false,                 -- blocked on await
      emitting = false,                 -- blocked on emit
      canrun   = nil,                   -- stack level of last emission
   }
   local bhv = setmetatable (t, behavior)
   if parent == nil then        -- this is root
      bhv.root = bhv
      bhv.co2bhv = {[co]=bhv}   -- maps coroutine to behavior
      bhv.stack = {}            -- stack of events
   else
      assert (getmetatable (parent) == behavior)
      bhv.root = assert (parent.root)
      bhv.root.co2bhv[co] = bhv
      table.insert (t.parent.children, bhv)
   end
   return bhv
end

-- Removes the i-th child of behavior.
local function bhvRemoveChild (bhv, i)
   local child = assert (table.remove (bhv.children, i))
   bhv.root.co2bhv[child.co] = nil
end


-- Private -----------------------------------------------------------------

-- Prints trace message (if tracing is on).
function behavior:_trace (fmt, ...)
   if self.root.trace then
      print (self.name..':'..(fmt):format (...))
   end
end

-- Dumps behavior tree to stdout.
function behavior:_dump (prefix)
   bhvDump (prefix or '', self.root, 0, self)
end

-- Returns a string representing behavior.
function behavior:__tostring ()
   local status
   if self.awaiting then
      status = ',awaiting'
   elseif self.emitting then
      status = (',emitting#%s'):format (assert (self.canrun))
   else
      status = ''
   end
   return ('%s (%s%s)'):format (self.name,
                                coroutine.status (self.co), status)
end

-- Tests whether behavior is root.
function behavior:_isRoot ()
   return self == self.root
end

-- Tests whether behavior is a parallel composition.
function behavior:_isPar (kind)
   if kind == nil then
      return self.kind ~= nil
   else
      return self.kind == kind
   end
end

-- Gets the behavior associated with coroutine co.
-- If CO is nil, gets the behavior associated with the current coroutine.
function behavior:_getBehavior (co)
   return self.root.co2bhv[co or coroutine.running ()]
end

-- Pushes event into event stack.
function behavior:_push (evt)
   table.insert (self.root.stack, assert (evt))
end

-- Pops event from event stack.
function behavior:_pop (evt)
   return assert (table.remove (self.root.stack))
end

-- Resumes behavior with the given arguments.
-- Throws an error in case of failure.
function behavior:_resume (...)
   assert (coroutine.status (self.co) == 'suspended')
   self:_trace ('_resume (%s)', args2str (...))
   local status, result = coroutine.resume (self.co, ...)
   if not status then
      error (result)
   end
   return result
end

-- Resumes behavior with the given arguments.
-- Calls the error handler in case of failure.
function behavior:_resumeSafe (...)
   assert (coroutine.status (self.co) == 'suspended')
   self:_trace ('_resumeSafe (%s)', args2str (...))
   local status, result = coroutine.resume (self.co, ...)
   if not status then
      assert (self.root.error) (self, result)
      return nil
   end
   return result
end

-- Broadcasts event to behavior tree.
function behavior:_broadcast (evt)
   assert (evt)
   if self.emitting then
      return                    -- skip behavior
   end
   self:_trace ('_broadcast (%s)', evt)
   if self.awaiting and not self:_isPar () then
      self:_trace (' ...trying to awake: %s', self)
      self:_resume (evt)
   else
      for _,child in ipairs (self.children) do
         child:_broadcast (evt)
      end
   end
end


-- Public ------------------------------------------------------------------

local function bhvInitBody (self)
   self:_trace (' ...init body started')
   while true do
      self:_trace (' ...init yielding')
      local evt = coroutine.yield ()
      self:_push (evt or true) -- push sentinel if there is no event
      if evt then
         self:_broadcast (evt)
      end
      self:_trace (' ...outer cycle started: %s', evt)
      while #self.stack > 0 do
         self:_trace (' ...innter cycle started: %s',
                      list2str (self.stack))
         local nblocked = 0
         local i = 1
         while i <= #self.children do
            local child = assert (self.children[i])
            local status = coroutine.status (child.co)
            if status ~= 'suspended' then -- zombie child
               assert (status == 'dead')
               self:_trace (' ...removing: %s', child)
               bhvRemoveChild (self, i)
            else
               self:_trace (' ...advancing: %s', child)
               local result = child:_resumeSafe ()
               if result == 'awaiting' then
                  i = i + 1
                  nblocked = nblocked + 1
               elseif result == 'emitting' then
                  break
               end
            end
         end
         if nblocked == #self.children then -- all children are blocked
            local out = self:_pop ()
            self:_trace (' ...popping: %s', args2str (out))
         end
         self:_trace (' ...inner cycle ended')
      end
      self:_trace (' ...outer cycle ended')
   end
end

--- Creates an initial behavior.
-- An initial behavior is the root behavior in a behavior tree.
-- @tparam[opt] table opts Options.
function behavior.init (opts)
   local opts = opts or {}
   --
   local name = opts.name or 'init'
   local error = opts.error or bhvError
   local trace = opts.trace or false
   --
   local init = assert (bhvNew (bhvInitBody, name))
   --
   init.error = error
   init.trace = trace
   init:_trace ('init %s', tab2str (opts))
   init:_resume (init)          -- bootstrap
   return init
end

--- Spawns behaviors.
-- @tparam table t List of functions to be used as behavior bodies.
function behavior:spawn (t)
   self:_trace ('spawn (#%d)', #t)
   if type (t) == 'function' then
      t = {t}
   end
   local curr = self:_getBehavior ()
   local parent = curr or self.root
   local root = parent.root
   assert (type (t) == 'table')
   for i,f in ipairs (t) do
      local child = assert (bhvNew (f, parent.name..'.spawn'..i, root))
      self.root:_trace (' ...adding: %s', child)
   end
   if not curr then             -- if not called by a behavior,
      self.root:_resume ()      --   bootstrap the spawn behaviors
   end
end

--- Awaits for event matching condition.
-- This function does nothing if it is not called by a behavior.
-- @param cond The condition to be awaited.
function behavior:await (cond)
   local curr = self:_getBehavior ()
   if curr == nil then
      return                    -- not called by a behavior, do nothing
   end
   curr:_trace ('await (%s)', cond)
   curr.awaiting = true
   local res
   repeat
      curr:_trace (' ...await yielding: awaiting')
      res = coroutine.yield ('awaiting')
   until res == cond
   curr:_trace (' ...await consumed')
   curr.awaiting = false
   coroutine.yield ()           -- ready to advance
end

--- Emits event.
-- @param evt the event to be emitted.
function behavior:emit (evt)
   local curr = self:_getBehavior ()
   if curr == nil then          -- external call
      self:_trace ('emit (%s) <<external>>', evt)
      self:_trace (' ...pushing: %s', args2str (evt))
      self.root:_push (evt)
      self.root:_broadcast (evt)
      self.root:_resume ()
   else                         -- internal call
      curr:_trace ('emit (%s) <<internal>>', evt)
      curr:_trace (' ...pushing: %s', args2str (evt))
      curr.canrun = #curr.root.stack
      curr.root:_push (evt)
      curr.emitting = true
      curr.root:_broadcast (evt)
      curr:_trace (' ...emit yielding: emitting')
      coroutine.yield ('emitting') -- start new cycle
      while curr.canrun ~= #curr.root.stack do
         curr:_trace (' ...emit yielding: awaiting')
         coroutine.yield ('awaiting') -- awaiting for stack level
      end
      curr:_trace (' ...emit consumed')
      curr.emitting = false
      curr.canrun = nil
   end
end

local function bhvParBody (par)
   par:_trace (' ...par/%s started', par.kind)
   while true do
      local result
      local i = 1
      while i <= #par.children do
         local child = assert (par.children[i])
         par:_trace (' ...advancing: %s', child)
         result = child:_resumeSafe ()
         par:_trace ('... advanced to: %s', result)
         if result == 'awaiting' then -- child blocked on await
            i = i + 1
         elseif result == 'emitting' then -- child executed an emit
            break
         elseif result == nil then
            if coroutine.status (child.co) == 'suspended' then
               goto continue    -- keep advancing child
            end
            assert (coroutine.status (child.co) == 'dead') -- child ended
            if par.kind == 'and' then
               par:_trace (' ...removing: %s', child)
               bhvRemoveChild (par, i)
               if #par.children == 0 then
                  goto done
               end
            elseif par.kind == 'or' then
               while #par.children > 0 do
                  par:_trace (' ...removing: %s', par.children[1])
                  child = bhvRemoveChild (par, 1)
               end
               goto done
            else
               error'should not get here'
            end
         else
            error'should not get here'
         end
         ::continue::
      end
      coroutine.yield (result)
   end
   ::done::
   par:_trace (' ...par/%s ended', par.kind)
   return
end

function behavior:_par (kind, t)
   assert (kind == 'or' or kind == 'and')
   assert (type (t) == 'table')
   if #t == 0 then
      return                    -- no functions, do nothing
   end
   local curr = self:_getBehavior ()
   local parent = curr or self
   local par = assert (bhvNew (bhvParBody, parent.name..'.'..kind, parent))
   par.kind = kind
   if not curr then
      self:_trace ('par/%s (#%d) <<external>>', par.kind, #t)
   else
      self:_trace ('par/%s (#%d) <<internal>>', par.kind, #t)
   end
   for i,f in ipairs (t) do
      local child = assert (bhvNew (f, par.name..'.'..i, par))
      self:_trace (' ...adding: %s', child)
   end
   if not curr then             -- external call
      par:_resume (par)
   else                         -- internal call
      while coroutine.status (par.co) ~= 'dead' do
         self:_trace (' ...advancing: %s', par)
         coroutine.yield (par:_resume (par))
      end
   end
end

--- Runs behaviors in a par/or composition.
-- The par/and composition executes until *any* of its behaviors terminates.
-- @tparam table t List of functions to be used as behavior bodies.
function behavior:parOr (t)
   return self:_par ('or', t)
end

--- Runs behaviors in a par/and composition.
-- The par/and composition executes until *all* its behaviors terminate.
-- @tparam table t List of functions to be used as behavior bodies.
function behavior:parAnd (t)
   return self:_par ('and', t)
end

return behavior
