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
      <property name="delay1" value="1s"/>
      <property name="delay2" value="2s"/>
      <property name="delay3" value="1s"/>
    </media>
    <media id="img" src="samples/gnu.png">
      <property name="bounds" value="0%,0%,50%,50%"/>
      <property name="explicitDur" value="5s"/>
      <property name="zIndex" value="1"/>
    </media>
    <link xconnector="onBeginStartDelay">
      <bind role="get" component="time" interface="delay1"/>
      <bind role="onBegin" component="time"/>
      <bind role="start" component="img">
        <bindParam name="delay" value="$get"/>
      </bind>
    </link>
    <link xconnector="onBeginStopDelay">
      <bind role="get" component="time" interface="delay2"/>
      <bind role="onBegin" component="img"/>
      <bind role="stop" component="img">
        <bindParam name="delay" value="$get"/>
      </bind>
    </link>
    <link xconnector="onEndStopDelay">
      <bind role="get" component="time" interface="delay3"/>
      <bind role="onEnd" component="img"/>
      <bind role="stop" component="time">
        <bindParam name="delay" value="$get"/>
      </bind>
    </link>
  </body>
</ncl>
