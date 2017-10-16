<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginStart">
        <simpleAction role="start"/>
        <simpleCondition role="onBegin"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="timer"/>
    <media id="timer">
      <area id="a1" begin="1s"/>
    </media>
    <media id="m" src="samples/night.avi">
      <property name="bounds" value="25%,25%,50%,50%"/>
    </media>
    <link xconnector="onBeginStart">
      <bind role="start" component="m"/>
      <bind role="onBegin" component="timer" interface="a1"/>
    </link>
  </body>
</ncl>
