<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginStart">
        <simpleCondition role="onBegin"/>
        <simpleAction role="start"/>
      </causalConnector>
      <causalConnector id="onBeginPause">
        <simpleCondition role="onBegin"/>
        <simpleAction role="pause"/>
      </causalConnector>
      <causalConnector id="onBeginResume">
        <simpleCondition role="onBegin"/>
        <simpleAction role="resume"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start0" component="timer"/>
    <media id="timer">
      <area id="a1" begin="1s"/>
      <area id="a2" begin="2s"/>
      <area id="a3" begin="3s"/>
      <area id="a4" begin="4s"/>
    </media>
    <media id="vid" src="samples/clock.ogv">
      <property name="bounds" value="25%,25%,50%,50%"/>
    </media>
    <link xconnector="onBeginStart">
      <bind role="onBegin" component="timer" interface="a1"/>
      <bind role="start" component="vid"/>
    </link>
    <link xconnector="onBeginPause">
      <bind role="onBegin" component="timer" interface="a2"/>
      <bind role="pause" component="vid"/>
    </link>
    <link xconnector="onBeginResume">
      <bind role="onBegin" component="timer" interface="a3"/>
      <bind role="resume" component="vid"/>
    </link>
  </body>
</ncl>
