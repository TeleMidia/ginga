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
end

local function bhvNew (f, name, parent)
   local co = assert (coroutine.create (f))
   local t = {
      co       = co,                    -- the underlying coroutine
      name     = name or tostring (co), -- behavior name
      parent   = parent,                -- parent behavior
      children = {},                    -- list of child trails
      trace    = false,                 -- whether to trace calls
   }
   local bhv = setmetatable (t, behavior)
   if parent == nil then        -- root
      bhv.root   = bhv
      bhv.co2bhv = {[co]=bhv}   -- maps coroutine to behavior
      bhv.stack  = {}           -- stack of events
   else
      assert (getmetatable (parent) == behavior)
      bhv.root = assert (parent.root)
      bhv.root.co2bhv[co] = bhv
      table.insert (t.parent.children, bhv)
   end
   return bhv
end

local function bhvRemoveChild (bhv, i)
   local child = assert (table.remove (bhv.root.children, i))
   bhv.root.co2bhv[child.co] = nil
end

local function dump (prefix, node, indent, highlight)
   local me = ('  '):rep (indent or 0)..tostring (node)
   if node == highlight then
      me = me..'*'
   end
   if node:_isRoot () then
      local evts = {}
      for _,v in ipairs (node.stack) do
         table.insert (evts, tostring (v))
      end
      me = me .. (' #%d: {%s}')
         :format (#node.stack, table.concat (evts, ','))
   end
   print (prefix..me)
   for _,child in ipairs (node.children) do
      dump (prefix, child, indent + 1, highlight)
   end
end

-- Private -----------------------------------------------------------------

function behavior:_trace (fmt, ...)
   if self.trace then
      print (self.name..'\t'..(fmt):format (...))
   end
end

function behavior:_dump (prefix)
   dump (prefix or '', self.root, 0, self)
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

function behavior:_isRoot ()
   return self == self.root
end

function behavior:_getBehavior (co)
   return self.root.co2bhv[co or coroutine.running ()]
end

function behavior:_push (evt)
   table.insert (self.root.stack, assert (evt))
end

function behavior:_pop (evt)
   return assert (table.remove (self.root.stack))
end

function behavior:_resume (evt)
   local status, result = coroutine.resume (self.co, evt)
   if not status then
      error (result)
   end
   return result
end

function behavior:_broadcast (evt)
   assert (evt)
   if self.emitting then
      return                    -- skip trail
   end
   self:_trace ('_broadcast (%s)', evt)
   if self.awaiting then
      self:_resume (evt)
   else
      for _,child in ipairs (self.children) do
         child:_broadcast (evt)
      end
   end
end

-- Public ------------------------------------------------------------------

function behavior.init (name)
   local func = function (self)
      while true do
         local evt = coroutine.yield ()
         self:_push (evt or true) -- push sentinel if there is no event
         if evt then
            self:_broadcast (evt)
         end
         self:_trace ('external cycle (%s)', evt)
         while #self.stack > 0 do
            self:_trace ('internal cycle (%s)', evt)
            --self:_dump ((' '):rep (4)..'\t|')
            local progress = false
            local i = 1
            while i <= #self.children do
               local child = assert (self.children[i])
               if not child.awaiting and not child.emitting then
                  progress = true -- child will advance
               end
               self:_trace ('... resuming trail: %s', child)
               local status, result = coroutine.resume (child.co)
               if not status then
                  error (('RESUME ERROR: %s: %s')
                        :format (tostring (child), tostring (result)))
               end
               if result == 'awaiting' then
                  i = i + 1
               elseif result == 'emitting' then
                  break
               elseif result == nil then
                  self:_trace ('... removing trail: %s', child)
                  assert (coroutine.status (child.co) == 'dead')
                  bhvRemoveChild (self, i)
               else
                  error'should not get here'
               end
            end
            if not progress then
               self:_trace ('... popping')
               self:_pop ()
            end
            self:_trace ('... internal cycle done')
         end
         self:_trace ('... external cycle done')
      end
   end
   local init = assert (bhvNew (func, name or 'init'))
   init:_resume (init)          -- bootstrap init
   return init
end

function behavior:spawn (t)
   self:_trace ('spawn (#%d)', #t)
   if type (t) == 'function' then
      t = {t}
   end
   assert (type (t) == 'table')
   for i,f in ipairs (t) do
      local child = bhvNew (f, self.name..':'..i, self.root)
      self.root:_trace ('... spawing trail: %s', child)
   end
   if self:_getBehavior () == nil then -- if not called by a behavior,
      self.root:_resume ()             --   bootstrap the spawn behaviors
   end
end

function behavior:await (cond)
   local curr = self:_getBehavior ()
   if curr == nil then
      return                    -- not called by a behavior, do nothing
   end
   curr:_trace ('await (%s)', cond)
   curr.awaiting = cond
   local res
   repeat
      curr:_trace ('... await yielding')
      res = coroutine.yield ('awaiting')
   until res == cond
   curr:_trace ('... await consumed')
   curr.awaiting = nil
   coroutine.yield ()           -- ready to continue
end

function behavior:emit (evt)
   local curr = self:_getBehavior ()
   if curr == nil then          -- external
      self:_trace ('emit external (%s)', evt)
      self.root:_push (evt)
      self.root:_broadcast (evt)
      self.root:_resume ()
   else                         -- internal
      curr:_trace ('emit internal (%s)', evt)
      curr.canrun = #curr.root.stack
      curr.root:_push (evt)
      curr.emitting = evt
      curr.root:_broadcast (evt)
      curr:_trace ('... emit yielding')
      coroutine.yield ('emitting') -- start new cycle
      while curr.canrun ~= #curr.root.stack do
         curr:_trace ('... emit yielding')
         coroutine.yield ('awaiting') -- awaiting for stack level
      end
      curr:_trace ('... emit consumed')
      curr.canrun = nil
      curr.emitting = nil
   end
end

return behavior
