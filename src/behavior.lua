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
   behavior.__index  = behavior
   behavior._debugOn = true
end

local args2str = function (...)
   local t = {}
   for _,v in ipairs {...} do
      table.insert (t, tostring (v))
   end
   return table.concat (t, ',')
end

function behavior._new (f, name, parent)
   local co = assert (coroutine.create (f))
   local t = {
      co       = co,
      name     = name or tostring (co),
      parent   = parent,
      children = {},
   }
   local bhv = setmetatable (t, behavior)
   if not parent then           -- this is a root behavior
      bhv.root = bhv
      bhv._co2bhv = {[co]=bhv}
   else
      assert (getmetatable (parent) == behavior)
      table.insert (t.parent.children, bhv)
      bhv.root = assert (parent.root)
      bhv.root._co2bhv[co] = bhv
   end
   return bhv
end

function behavior:__tostring ()
   return ('%s (%s)'):format (self.name, coroutine.status (self.co))
end

function behavior:_debug (fmt, ...)
   if behavior._debugOn then
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
   print (me)
   for _,child in ipairs (node.children) do
      _dumpTree (child, indent + 1, highlight)
   end
end

function behavior:_dumpTree ()
   _dumpTree (self.root, 0, self)
end

function behavior:_getBehavior (co)
   return assert (self.root._co2bhv)[co or coroutine.running ()]
end

function behavior:_broadcast (...)
   self:_debug1 ('_broadcast (%s)', args2str (...))
   if coroutine.status (self.co) == 'suspended' then
      assert (coroutine.resume (self.co, ...))
   else
      error ('internal broadcast is not implemented')
   end
end

-- Exported functions ------------------------------------------------------

function behavior.init (name)
   local func = function (self)
      while true do
         local t = {coroutine.yield ()}
         self:_debug1 ('cycle (%s)', args2str (table.unpack (t)))
         local i = 1
         while i <= #self.children do
            local child = assert (self.children[i])
            self:_debug2 ("resuming child '%s'", child)
            local ret, err = coroutine.resume (child.co, table.unpack (t))
            if not ret then
               print ('INIT RESUME ERROR: '..tostring (child)
                         ..': '..tostring (err))
            end
            if err ~= nil then
               print ('>>>', err)
            end
            if coroutine.status (child.co) == 'dead' then
               self:_debug2 ("removing child '%s'", child)
               self._co2bhv[child.co] = nil
               table.remove (self.children, i)
            else
               i = i + 1
            end
         end
         self:_debug2'done'
      end
   end
   local init = assert (behavior._new (func, name or 'init'))
   init:_broadcast (init)       -- bootstrap init
   return init
end

function behavior:spawn (t)
   assert (self.parent == nil)
   self:_debug1 ('spawn {%s}', args2str (table.unpack (t)))
   if type (t) == 'function' then
      t = {t}
   end
   assert (type (t) == 'table')
   for i,f in ipairs (t) do
      local child = behavior._new (f, self.name..':'..i, self.root)
      self:_debug2 ("adding child '%s'", child)
   end
   self:_broadcast ()           -- bootstrap the spawn behaviors
end

function behavior:await (cond)
   local curr = assert (self:_getBehavior ())
   curr:_debug1 ('await (%s)', cond)
   local res
   repeat
      curr:_debug2 ("yielding")
      res = coroutine.yield ()
   until res == cond
   curr:_debug2 ("awaking with '%s'", res)
end

function behavior:emit (evt)
   local curr = assert (self:_getBehavior ())
   curr:_debug1 ('emit (%s)', evt)
   self.root:_broadcast (evt)
   --coroutine.yield ({curr.co, evt})
end

return behavior
