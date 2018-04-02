<ncl>
  <head>
    <connectorBase>
      <causalConnector id='onBeginStart'>
        <simpleCondition role='onBegin'/>
        <simpleAction role='start'/>
      </causalConnector>
      <causalConnector id='onEndStart'>
        <simpleCondition role='onEnd'/>
        <simpleAction role='start'/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id='start0' component='m1' interface='a1'/>
    <media id='m1' src='samples/clock.ogv'>
      <property name="zIndex" value="0"/>
      <property name='bounds' value='0%,0%,50%,50%'/>
      <area id='a1' begin='2s'/>
      <area id='a2' begin='4s'/>
      <area id='a3' begin='6s'/>
    </media>
    <media id='m2' src='samples/bunny.ogg'>
      <property name="zIndex" value="1"/>
      <property name='bounds' value='50%,50%,50%,50%'/>
    </media>
    <link xconnector='onBeginStart'>
      <bind role='onBegin' component='m1' interface='a2'/>
      <bind role='start' component='m2'/>
    </link>
  </body>
</ncl>