require "fps"

function handler (evt)
  if evt.class == 'ncl' and
     evt.type == 'presentation' and
     evt.action == 'start' then
 
    -- 1s after @lambda starts 'la1'
    if evt.label == '' then
      event.timer (1000,
        function () 
          evt.label = 'la1'
          event.post ( evt )
        end)
    -- 3s after 'la1' started, stop it
    elseif evt.label == 'la1' then
      event.timer (3000,
        function () 
          evt.action = 'stop'
          event.post (evt)
        end)
    end
  end
end

event.register (handler)

