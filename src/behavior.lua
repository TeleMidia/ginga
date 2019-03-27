local assert       = assert
local coroutine    = coroutine
local debug        = debug
local error        = error
local getmetatable = getmetatable
local ipairs       = ipairs
local print        = print
local setmetatable = setmetatable
local table        = table
local tostring     = tostring
local type         = type

local _ENV = nil
local behavior = {}
do
   behavior.__index    = behavior
   behavior._debugging = false
end

local args2str = function (...)
   local t = {}
   for _,v in ipairs {...} do
      table.insert (t, tostring (v))
   end
   return table.concat (t, ',')
end

-- Creates a new behavior.
local function behavior_new (f, name, parent)
   local co = assert (coroutine.create (f))
   local t = {
      co       = co,                    -- the underlying coroutine
      name     = name or tostring (co), -- behavior name
      parent   = parent,                -- parent behavior
      children = {},                    -- list of children
   }
   local bhv = setmetatable (t, behavior)
   if parent == nil then        -- root
      bhv.root    = bhv
      bhv._co2bhv = {[co]=bhv}  -- maps coroutine to behavior
      bhv.stack   = {}          -- stack of events
      bhv.inq     = {}          -- input queue
      bhv.outq    = {}          -- output queue
   else
      assert (getmetatable (parent) == behavior)
      table.insert (t.parent.children, bhv)
      bhv.root = assert (parent.root)
      bhv.root._co2bhv[co] = bhv
   end
   return bhv
end

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

function behavior:_debug (fmt, ...)
   if behavior._debugging then
      print (self.name..'\t'..(fmt):format (...))
   end
end

function behavior:_debug1 (fmt, ...)
   self:_debug ('-~- '..fmt, ...)
end

function behavior:_debug2 (fmt, ...)
   self:_debug ('... '..fmt, ...)
end

local function _dumpTree (node, indent, highlight)
   local me = ('  '):rep (indent or 0)..tostring (node)
   if node == highlight then
      me = me..'*'
   end
   if node.parent == nil then
      me = me .. (' #%d'):format (#node.stack)
   end
   print (me)
   for _,child in ipairs (node.children) do
      _dumpTree (child, indent + 1, highlight)
   end
end

function behavior:_dumpTree ()
   if behavior._debugging then
      _dumpTree (self.root, 0, self)
   end
end

function behavior:_getBehavior (co)
   return self.root._co2bhv[co or coroutine.running ()]
end

function behavior:_push (evt)
   table.insert (self.root.stack, assert (evt))
end

function behavior:_pop (evt)
   return assert (table.remove (self.root.stack))
end

-- Start a new cycle with the given external event.
-- This function should not be called by a behavior.
function behavior:_cycle (evt)
   self:_debug1 ('_cycle (%s)', evt)
   assert (self:_getBehavior () == nil)
   assert (coroutine.resume (self.root.co, evt))
end

function behavior:_broadcast (evt)
   assert (evt)
   if self.emitting then
      return                    -- nothing to do
   end
   self:_debug1 ('_broadcast (%s)', evt)
   if self.awaiting then
      assert (coroutine.resume (self.co, evt))
   else
      for _,child in ipairs (self.children) do
         child:_broadcast (evt)
      end
   end
end

-- Exported functions ------------------------------------------------------

function behavior.init (name)
   local func = function (self)
      while true do
         local evt = coroutine.yield ()
         self:_push (evt or true)
         if evt then
            self:_broadcast (evt)
         end
         --
         self:_debug1 ('external cycle (%s)', evt)
         --
         while #self.stack > 0 do
            --
            self:_debug1 ('internal cycle (%s)', evt)
            self:_dumpTree ()
            --
            local progress = false
            local i = 1
            while i <= #self.children do
               local child = assert (self.children[i])
               if not child.awaiting and not child.emitting then
                  progress = true
               end
               --
               self:_debug2 ("resuming child '%s'", child)
               --
               local status, result = coroutine.resume (child.co)
               if not status then
                  error ('RESUME ERROR: '..tostring (child)
                            ..': '..tostring (result))
               end
               if result == 'awaiting' then
                  i = i + 1
               elseif result == 'emitting' then
                  break
               elseif result == nil then -- finished
                  self:_debug2 ("removing child '%s'", child)
                  assert (coroutine.status (child.co) == 'dead')
                  self._co2bhv[child.co] = nil
                  table.remove (self.children, i)
               else
                  error'should not get here'
               end
            end
            if not progress then
               self:_debug2'popping'
               self:_pop ()
            end
            self:_debug2'internal cycle done'
         end
         self:_debug2'external cycle done'
      end
   end
   local init = assert (behavior_new (func, name or 'init'))
   init:_cycle (init)           -- bootstrap init
   return init
end

function behavior:spawn (t)
   assert (not self:_getBehavior (),
           'should not be called by a behavior')
   --
   self:_debug1 ('spawn {%s}', args2str (table.unpack (t)))
   if type (t) == 'function' then
      t = {t}
   end
   assert (type (t) == 'table')
   for i,f in ipairs (t) do
      local child = behavior_new (f, self.name..':'..i, self.root)
      self:_debug2 ("adding child '%s'", child)
   end
   self:_cycle ()               -- bootstrap the spawn behaviors
end

function behavior:cycle (evt)
   assert (not self:_getBehavior (),
           'should not be called by a behavior')
   --
   -- pop event from INQ cycle it
   self:_cycle (evt)
end

function behavior:await (cond)
   local curr = self:_getBehavior ()
   assert (curr, 'should be called by a behavior')
   --
   curr:_debug1 ('await (%s)', cond)
   curr.awaiting = cond
   local res
   repeat
      curr:_debug2 ("await yielding")
      res = coroutine.yield ('awaiting')
   until res == cond
   curr:_debug2 ("await '%s' consumed", res)
   curr.awaiting = nil
   coroutine.yield ()
end

function behavior:emit (evt)
   local curr = self:_getBehavior ()
   assert (curr, 'should be called by a behavior')
   --
   curr:_debug1 ('emit (%s)', evt)
   curr:_debug2 ('internal emit')
   curr.canrun = #curr.root.stack
   curr.root:_push (evt)
   curr.emitting = evt
   curr.root:_broadcast (evt)
   curr:_debug2 ("emit yielding")
   coroutine.yield ('emitting')
   while curr.canrun ~= #curr.root.stack do
      curr:_debug2 ("emit yielding")
      coroutine.yield ('awaiting')
   end
   curr:_debug2 ('awaking from emit')
   curr.canrun = nil
   curr.emitting = nil
end

return behavior
