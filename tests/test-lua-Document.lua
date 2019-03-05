local ginga = require ('ginga')

-- createDocument
do
   local checkEmpty = function (doc)
      assert (doc.object)

      local root = doc.object.__root__
      assert (doc.object.__root__)
      assert (root.doc == doc)
      assert (root.parent == nil)
      assert (root.type == 'context')
      assert (root.id == '__root__')
      assert (root.event.presentation['@lambda'])
      assert (doc.event['__root__@lambda'])

      local sett = doc.object.__settings__
      assert (sett.doc == doc);
      assert (sett.parent == root);
      assert (sett.type == 'media')
      assert (sett.id == '__settings__')
      assert (sett.event.presentation['@lambda'])
      assert (sett.event.attribution['service.currentFocus'])
   end
   local d1 = ginga.createDocument ()
   local d2 = ginga.createDocument ()
   checkEmpty (d1)
   checkEmpty (d2)
   assert (d1 ~= d2)
end

-- __tostring

-- _getUnderlyingObject

-- getObjects

-- getObject

-- getRoot

-- getSettings

-- createObject

-- createEvent (type, objId, evtId)

-- createEvent (qualId)
do
   local doc
   local evt

   doc = ginga.createDocument ()
   assert (doc)

   evt = doc:createEvent ('__root__@x')
   assert (evt)
end

-- getEvent
do
   local doc
   local evt

   doc = ginga.createDocument ()
   assert (doc)

   -- bad format
   evt = doc:getEvent ('__root__')
   assert (evt == nil)

   -- no such event
   evt = doc:getEvent ('__root__@x')
   assert (evt == nil)

   -- presentation
   evt = doc:getEvent ('__root__@lambda')
   assert (evt)
   assert (evt == doc.event['__root__@lambda'])
   assert (evt.object == doc.object.__root__)
   assert (evt.type == 'presentation')
   assert (evt.id == '@lambda')

   -- attribution
   evt = doc:createEvent ('attribution', '__settings__', 'x')
   assert (evt)
   assert (evt.object == doc.object.__settings__)
   assert (evt.type == 'attribution')
   assert (evt.id == 'x')

   -- selection
   evt = doc:createEvent ('selection', '__settings__', 'RED')
   assert (evt)
   assert (evt.object == doc.object.__settings__)
   assert (evt.type == 'selection')
   assert (evt.id == 'RED')
end
