<?xml version="1.0" encoding="utf-8"?>
<ncl>
  <body>
    <port id='start' component='m'/>
    <media id='m' src='samples/gnu.png'>
      <script>
      <![CDATA[
        local m =...
        m.property.width = .25
        m.property.height = .25
        m.property.top = .35
        m.property.left = .35
        parOr {
          function ()
            while true do
              await {key='CURSOR_TOP', press=true}
              m.property.top = m.property.top - .1
            end
          end,
          function ()
            while true do
              await {key='CURSOR_BOTTOM', press=true}
              m.property.top = m.property.top + .1
            end
          end,
          function ()
            while true do
              await {key='CURSOR_LEFT', press=true}
              m.property.left = m.property.left - .1
            end
          end,
          function ()
            while true do
              await {key='CURSOR_RIGHT', press=true}
              m.property.left = m.property.left + .1
            end
          end,
          function ()
            await {key='q', press=true}
          end,
        }
        os.exit (0)
      ]]>
      </script>
    </media>
  </body>
</ncl>
