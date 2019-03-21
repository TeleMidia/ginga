<ncl>
  <body>
    <port id="start" component="m"/>
    <media id="m" src="samples/bunny.ogg">
      <property name="left" value="50%"/>
      <property name="left" value="25%"/>
      <property name="top" value="50%"/>
      <property name="top" value="25%"/>
      <script>
        local m = ... -- gets the container object (m)
        await (.5)
        m.property.left = 0
        await (.5)
        m.property.top = 0
        await (.5)
        m.property.width = 100
        await (.5)
        m.property.height = 100
        await (.5)
        m.property.right = 0
        await (.5)
        m.property.bottom = 0
        await (.5)
        m.property.left = 0.
        await (.5)
        m.property.top = '0%'
        await (.5)
        m.property.width  = '50%'
        m.property.height = .5
        local x, y = 0, 0
        while true do
           local t = m.time
           await (0)
           local dt = (m.time - t) / 1000000
           x = x + 300 * dt
           y = y + 300 * dt
           m.property.left = x..'px'
           m.property.top = y..'px'
           if m.property.left > _D.settings.property.width
              or m.property.top > _D.settings.property.height then
              break
           end
        end
        m.property.left = '25%'
        m.property.top = .25
      </script>
    </media>
  </body>
</ncl>
