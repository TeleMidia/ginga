<ncl>
  <head>
    <behavior>
    <![CDATA[
      local doc = ...
      parOr {
         function ()
            print ('t1', 'start')
            local wait = 1000000
            while true do
               local t = await {target=doc, time=wait}
               print ('now: '..math.floor (doc.time/1000000)
                         ..' ('..doc.time..')',
                      'next: '..doc.time + wait)
            end
            print ('t1', 'end')
         end,
         function ()
            print ('t2', 'start')
            await {target=doc, time=100*1000000}
            print ('t2', 'end')
         end,
      }
      print ('done')
    ]]>
    </behavior>
  </head>
</ncl>
