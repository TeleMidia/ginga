local assert       = assert
local coroutine    = coroutine
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
   behavior.__index     = behavior
   --behavior.__metatable = 'not your business'
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

function behavior:_broadcast (...)
   assert (coroutine.resume (self.root.co, ...))
end

-- Exported functions.

function behavior:getBehavior (co)
   local co2bhv = assert (self.root._co2bhv)
   return co2bhv[co]
end

function behavior:init (name)
   local func = function (init)
      while true do
         local t = {coroutine.yield ()}
         local i = 1
         while i <= #init.children do
            local child = assert (init.children[i])
            local ret, err = coroutine.resume (child.co, table.unpack (t))
            if not ret then
               print ('INIT ERROR: '..tostring (child)
                         ..': '..tostring (err))
            end
            if coroutine.status (child.co) == 'dead' then
               init._co2bhv[child.co] = nil
               table.remove (init.children, i)
            else
               i = i + 1
            end
         end
      end
   end
   local init = assert (behavior._new (func, name or 'init'))
   init:_broadcast (init)       -- bootstrap init
   return init
end

function behavior:spawn (t)
   if type (t) == 'function' then
      t = {t}
   end
   assert (type (t) == 'table')
   for i,f in ipairs (t) do
      behavior._new (f, self.name..':'..i, self.root)
   end
   self:_broadcast ()           -- bootstrap the spawn behaviors
end

return behavior
