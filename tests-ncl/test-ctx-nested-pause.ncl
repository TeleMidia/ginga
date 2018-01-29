<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginPause">
        <connectorParam name="var"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="pause"/>
      </causalConnector>
      <causalConnector id="onBeginResume">
        <connectorParam name="var"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="resume"/>
      </causalConnector>
      <causalConnector id="onPauseSet">
        <connectorParam name="var"/>
        <simpleCondition role="onPause"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start1" component="time"/>
    <port id="start2" component="ctx"/>
    <media id="time">
      <area id="a1" begin="2s"/>
      <area id="a2" begin="3s"/>
    </media>
    <context id="ctx">
      <port id="start0a" component="dummy"/>
      <media id="dummy" src="samples/bunny.ogg"/>
    </context>
    <link xconnector="onBeginPause">
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="pause" component="ctx" />
    </link>
    <link xconnector="onBeginResume">
      <bind role="onBegin" component="time" interface="a2"/>
      <bind role="resume" component="ctx" />
    </link>
  </body>
</ncl>
