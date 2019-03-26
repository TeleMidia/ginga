local assert = assert
local print  = print

local ginga = require'ginga'
_ENV = nil
do
   local doc = assert (ginga.createDocument ())
   local m = doc:createObject ('media', 'm')
   doc:_dump ()
   doc.settings.property.width = 800
   doc.settings.property.height = 600
end
