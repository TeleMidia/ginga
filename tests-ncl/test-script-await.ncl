<ncl>
  <head>
    <script>
      local doc = ...
      while true do
        local before = doc.time
        local t = doc._await {object=doc, time=1000000}
        print (doc.time, doc.delta)
      end
    </script>
  </head>
</ncl>
