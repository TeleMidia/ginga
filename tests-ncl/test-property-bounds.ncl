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
    <port id="start" component="gnu"/>
    <media id="gnu" src="samples/gnu.png">
      <property name="bounds" value=".25,.25,.5,.5"/>
      <script>
        local gnu = ...
        assert (gnu == _D.object.gnu)        
        await {object=gnu, time=1000000}
        print (gnu.id, gnu.time)
      </script>
    </media>
  </body>
</ncl>
