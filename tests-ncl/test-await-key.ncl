<?xml version="1.0" encoding="utf-8"?>
<ncl>
  <body>
    <port id='start' component='m'/>
    <media id='m' src='samples/gnu.png'>
      <behavior>
      <![CDATA[
        local m =...
        m.property.width = .5
        m.property.height = .5
        m.property.top = .25
        m.property.left = .25
        parOr {
           function ()
              while true do
                 await {key='CURSOR_UP', type='press'}
                 m.property.top = m.property.top - .1
                 await {key='CURSOR_UP', type='release'}
                 m.property.top = m.property.top + .1
              end
           end,
           function ()
              while true do
                 await {key='CURSOR_DOWN', type='press'}
                 m.property.top = m.property.top + .1
                 await {key='CURSOR_DOWN', type='release'}
                 m.property.top = m.property.top - .1
              end
           end,
           function ()
              while true do
                 await {key='CURSOR_LEFT', type='press'}
                 m.property.left = m.property.left - .1
                 await {key='CURSOR_LEFT', type='release'}
                 m.property.left = m.property.left + .1
              end
           end,
           function ()
              while true do
                 await {key='CURSOR_RIGHT', type='press'}
                 m.property.left = m.property.left + .1
                 await {key='CURSOR_RIGHT', type='release'}
                 m.property.left = m.property.left - .1
              end
           end,
           function ()
              await {key='q', type='press'} -- quit
              os.exit (0)
           end,
        }
      ]]>
      </behavior>
    </media>
  </body>
</ncl>
