local counter = 0
local dx, dy = canvas:attrSize()
function handler1 (evt)
   if evt.class=='ncl' and evt.type=='attribution' and evt.action=='start' and evt.name=='add' then 
      counter = counter + evt.value

      event.post {
         class   = 'ncl',
         type    = 'attribution',
         name    = 'add',
         action  = 'stop',
         value   = counter,
      }
   end
end

function handler2 (evt)
   if evt.class=='ncl' and evt.type=='presentation' and evt.action=='start' and evt.label=='fim' then 
      canvas:attrColor ('black')
      canvas:drawRect('fill',0,0,dx,dy)
      canvas:attrColor ('yellow')
      canvas:attrFont ('vera', 24, 'bold')
      canvas:drawText (10,10, 'O número de vezes que você trocou de ritmo foi: '..counter)
      canvas:flush()

      event.post {
         class   = 'ncl',
         type    = 'presentation',
         label   = 'fim',
         action  = 'stop',
      }
   end
end

event.register(handler1)
event.register(handler2)
