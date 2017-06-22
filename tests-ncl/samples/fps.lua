local event = event
local canvas = canvas
_ENV = nil

local FIRST = event.uptime ()
local LAST = FIRST
canvas:attrFont ('tiresias', 40, 'bold')
canvas:attrLineWidth (1.0)
local function redraw (e)
   local w, h = canvas:attrSize ()
   canvas:attrClip (0, 0, w, h)
   canvas:attrColor (0, 0, 0, 0)
   canvas:clear (0, 0, w, h)
   canvas:attrColor ('black')
   canvas:attrAntiAlias ('none')
   local n = 10
   for i=0,w/n do
      canvas:drawLine (i * n, 0, i * n, h)
   end
   for i=0,h/n do
      canvas:drawLine (0, i * n, w, i * n)
   end
   local diff = e.time - LAST
   LAST = e.time
   local text = ('%.2fs\n%.2f fps'):format ((LAST-FIRST)/1000,1000/diff)
   local tw, th = canvas:measureText (text)
   canvas:attrAntiAlias ('default')
   canvas:attrColor ('yellow')
   canvas:drawText ((w-tw)/2, (h-th)/2, text)
   canvas:attrColor ('navy')
   canvas:drawText ((w-tw)/2 - 1, (h-th)/2 - 1, text)
   canvas:flush ()
   event.post ('in', {class='user', time=event.uptime ()})
end
event.register (redraw, {class='user'})
event.register (
   function (e)
      event.post ('in', {class='user', time=event.uptime ()})
   end,
   {class='ncl', type='presentation', action='start'}
)
