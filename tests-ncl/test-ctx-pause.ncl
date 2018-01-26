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
    <port id="p1" component="ctx1"/>
    <port id="p2" component="time"/>
    <media id="time">
      <area id="a1" begin="2s"/>
      <area id="a2" begin="3s"/>
    </media>
    <context id="ctx1">
      <port id="p3" component="ctx2"/>
      <context id="ctx2">
        <port id="start0a" component="dummy"/>
        <media id="dummy" src="samples/bunny.ogg"/>
      </context>
    </context>
    <link xconnector="onBeginPause">
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="pause" component="ctx1" />
    </link>
    <link xconnector="onBeginResume">
      <bind role="onBegin" component="time" interface="a2"/>
      <bind role="resume" component="ctx1" />
    </link>
  </body>
</ncl>