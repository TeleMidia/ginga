<ncl>
  <head>
    <script>
      local doc = ...
      local wait = 1000000
      while true do
        local t = doc._await {object=doc, time=wait}
        print ('now: '..math.floor (doc.time/1000000)..' ('..doc.time..')',
               'next: '..doc.time + wait - doc.delta,
               'delta: '..doc.delta)
      end
    </script>
  </head>
</ncl>
