<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginPause">
        <connectorParam name="var"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="pause"/>
      </causalConnector>
      <causalConnector id="onPauseSet">
        <connectorParam name="var"/>
        <simpleCondition role="onPause"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body id="self">
    <port id="start" component="time"/>
    <media id="time">
      <property name="size" value="100%,100%"/>
      <property name="background" value="red"/>
      <area id="a1" begin="1s"/>
    </media>
    <link xconnector="onBeginPause">
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="pause" component="self" />
    </link>
    <link xconnector="onPauseSet">
      <bind role="onPause" component="self"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
  </body>
</ncl>
