local ginga = require ('ginga')
do
   local doc = assert (ginga.createDocument ())
   local m = doc:createObject ('media', 'm')
   doc:_dump ()
   doc.settings.property.width = 800
   doc.settings.property.height = 600

   m.lambda:transition ('start')
end
