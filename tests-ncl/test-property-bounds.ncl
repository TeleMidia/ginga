<ncl>
  <head>
    <script>
      local doc = ...
      assert (doc == _D)
      await (1.5)
      print ('doc', doc.time)
    </script>
  </head>
  <body>
    <port id="start" component="m"/>
    <media id="m" src="samples/bunny.ogg">
      <property name="bounds" value=".25,.25,.5,.5"/>
      <script>
        local m = ...
        assert (m == _D.object.m)        
        await {object=m, time=1000000}
        print (m.id, m.time)
      </script>
    </media>
  </body>
</ncl>
