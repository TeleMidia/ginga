local event = event
local canvas = canvas
local prop = 'left'
local max_value = 500

canvas:attrColor (255, 255, 255)
canvas:drawRect ('fill', 0, 0, 100, 100)
canvas:flush ()

function update_prop (evt)
  local new_value = math.random (max_value)
  print ("update " .. prop .. " to ", new_value)

  event.post {
    class  = 'ncl',
    type   = 'attribution',
    name   = prop,
    value  = new_value,
    action = 'start'
  }
    
  event.post {
    class  = 'ncl',
    type   = 'attribution',
    name   = prop,
    value  = new_value,
    action = 'stop'
  }

  event.timer(1000, function () event.post {class = 'user'} end )
end

event.register (update_prop, {class='user'})

event.register (
   function (e)
      event.post ('in', {class='user'})
   end,
   {class='ncl', type='presentation', action='start'}
)

