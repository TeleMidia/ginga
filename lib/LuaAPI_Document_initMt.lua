-- Dumps graph rooted at node.
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
      data._object   = {}       -- objects indexed by id
      data._event    = {}       -- events indexed by qualified id
      data._parents  = {}       -- table of parents indexed by object
      data._children = {}       -- table of children indexed by object

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

      return saved_attachData (self, data, funcs)
   end

   -- Initializes private data.
   local saved_init = assert (mt._init)
   mt._init = function (self)
      assert (self:createObject ('context', '__root__'))
      assert (self:createObject ('media', '__settings__'))
      return saved_init (self)
   end

   -- Finalizes private data.
   local saved_fini = assert (mt._fini)
   mt._fini = function (self)
      return saved_fini (self)
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

   -- Gets the table of parents of obj.
   mt._getParents = function (self, obj)
      assert (obj.document == self)
      local t = {}
      for k,_ in pairs (self.parents[obj] or {}) do
         table.insert (t, k)
      end
      return t
   end

   -- Gets the table of children of obj.
   mt._getChildren = function (self, obj)
      assert (obj.document == self)
      assert (obj:isComposition ())
      local t = {}
      for k,_ in pairs (assert (self.children[obj])) do
         table.insert (t, k)
      end
      return t
   end

   -- Adds obj to parent.
   mt._addChild = function (self, parent, obj)
      assert (parent.document == self)
      assert (obj.document == self)
      assert (parent:isComposition ())
      local tchildren = assert (self.children[parent])
      tchildren[obj] = true
      local tparents = assert (self.parents[obj])
      tparents[parent] = true
   end

   -- Removes obj from parent.
   mt._removeChild = function (self, parent, obj)
      assert (parent.document == self)
      assert (obj.document == self)
      local tchildren = assert (self.children[parent])
      tchildren[obj] = nil
      local tparents = assert (self.parents[obj])
      tparents[parent] = nil
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

   -- Exported functions ---------------------------------------------------

   -- Document::getObjects().
   mt.getObjects = function (self)
      local t = {}
      for _,v in pairs (self.object) do
         table.insert (t, v)
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
      for _,v in pairs (self.event) do
         table.insert (t, v)
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
end
