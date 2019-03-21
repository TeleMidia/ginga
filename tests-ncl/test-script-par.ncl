<ncl>
  <head>
    <script>
      local doc = ...
      doc._par {
         function ()
            print (doc.time, 't1 started')
            await (1)
            print (doc.time, 't1 continued')
            await (3)
            print (doc.time, 't1 ended')
         end,
         function ()
            print (doc.time, 't2 started')
            await (2)
            print (doc.time, 't2 ended')
         end
      }
      print (doc.time, 't1 and t2 joined')
    </script>
  </head>
</ncl>
