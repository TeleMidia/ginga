<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginStartDelay">
        <connectorParam name="delay"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="start" delay="$delay"/>
      </causalConnector>
      <causalConnector id="onBeginStopDelay">
        <connectorParam name="delay"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="stop" delay="$delay"/>
      </causalConnector>
      <causalConnector id="onEndStopDelay">
        <connectorParam name="delay"/>
        <simpleCondition role="onEnd"/>
        <simpleAction role="stop" delay="$delay"/>
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
    <link xconnector="onBeginStartDelay">
      <bind role="onBegin" component="time"/>
      <bind role="start" component="img">
        <bindParam name="delay" value="1s"/>
      </bind>
    </link>
    <link xconnector="onBeginStopDelay">
      <bind role="onBegin" component="img"/>
      <bind role="stop" component="img">
        <bindParam name="delay" value="2s"/>
      </bind>
    </link>
    <link xconnector="onEndStopDelay">
      <bind role="onEnd" component="img"/>
      <bind role="stop" component="time">
        <bindParam name="delay" value="1s"/>
      </bind>
    </link>
  </body>
</ncl>
