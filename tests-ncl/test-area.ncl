<?xml version="1.0" encoding="utf-8"?>
<ncl>
  <body>
    <port id='start' component='m'/>
    <media id='m' src='samples/gnu.png'>
      <area id='a1'/>
      <area id='a2' begin='1s'/>
      <area id='a3' end='1s'/>
      <area id='a4' begin='1s' end='2s'/>
      <behavior>
      <![CDATA[
        local m = ...
        local a1 = m.presentation.a1
        local a2 = m.presentation.a2
        local a3 = m.presentation.a3
        local a4 = m.presentation.a4

        parOr {
           function ()
              assert (m.lambda.state == 'sleeping')
              assert (a1.state == 'sleeping')
              assert (a2.state == 'sleeping')
              assert (a3.state == 'sleeping')
              assert (a4.state == 'sleeping')

              await (0) -- await for the first tick
              assert (m.time == 0)
              assert (m.lambda.state == 'occurring')
              assert (a1.state == 'occurring')
              assert (a2.state == 'sleeping')
              assert (a3.state == 'occurring')
              assert (a4.state == 'sleeping')

              await (1)
              assert (m.time == 1000)
              assert (m.lambda.state == 'occurring')
              assert (a1.state == 'occurring')
              assert (a2.state == 'occurring')
              assert (a3.state == 'sleeping')
              assert (a4.state == 'occurring')

              await (1)
              assert (m.time == 2000)
              assert (m.lambda.state == 'occurring')
              assert (a1.state == 'occurring')
              assert (a2.state == 'occurring')
              assert (a3.state == 'sleeping')
              assert (a4.state == 'sleeping')

              m.lambda:transition ('stop')
              assert (a1.state == 'sleeping')
              assert (a2.state == 'sleeping')
              assert (a3.state == 'sleeping')
              assert (a4.state == 'sleeping')
              assert (m.time == 2000)
           end,
           function ()
              while true do
                 evt = await {}
                 if evt.statemachine then
                    local obj = assert (evt.target)
                    local sm = evt.statemachine
                    local trans = assert (evt.transition)
                    print (obj.time, sm.qualifiedId, trans)
                 end
              end
           end
        }
      ]]>
      </behavior>
    </media>
  </body>
</ncl>
