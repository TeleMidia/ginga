<ncl>
  <body>
    <port id="start" component="m"/>
    <media id="m" src="samples/bunny.ogg">
      <property name="bounds" value=".25,.25,.5,.5"/>
      <behavior>
      <![CDATA[
        local m = ...
        assert (m == _D.object.m)
        await {target=m, time=1000000}
        print (m.id, m.time)
      ]]>
      </behavior>
    </media>
  </body>
</ncl>
