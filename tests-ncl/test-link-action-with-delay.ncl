<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginStartDelay1s">
        <simpleCondition role="onBegin"/>
        <simpleAction role="start" delay="1s"/>
      </causalConnector>
      <causalConnector id="onBeginStopDelay2s">
        <simpleCondition role="onBegin"/>
        <simpleAction role="stop" delay="2s"/>
      </causalConnector>
      <causalConnector id="onEndStopDelay1s">
        <simpleCondition role="onEnd"/>
        <simpleAction role="stop" delay="1s"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="time"/>
    <media id="time">
      <property name="background" value="red"/>
      <property name="bounds" value="25%,25%,50%,50%"/>
    </media>
    <media id="img" src="samples/gnu.png">
      <property name="bounds" value="0%,0%,50%,50%"/>
      <property name="explicitDur" value="5s"/>
      <property name="zIndex" value="1"/>
    </media>
    <link xconnector="onBeginStartDelay1s">
      <bind role="onBegin" component="time"/>
      <bind role="start" component="img"/>
    </link>
    <link xconnector="onBeginStopDelay2s">
      <bind role="onBegin" component="img"/>
      <bind role="stop" component="img"/>
    </link>
    <link xconnector="onEndStopDelay1s">
      <bind role="onEnd" component="img"/>
      <bind role="stop" component="time"/>
    </link>
  </body>
</ncl>
