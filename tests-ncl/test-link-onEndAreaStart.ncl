<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onEndStart">
        <simpleAction role="start"/>
        <simpleCondition role="onEnd"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="timer"/>
    <media id="timer">
      <area id="a1" begin="1s" end="2s"/>
    </media>
    <media id="m" src="samples/night.avi">
      <property name="bounds" value="25%,25%,50%,50%"/>
    </media>
    <link xconnector="onEndStart">
      <bind role="start" component="m"/>
      <bind role="onEnd" component="timer" interface="a1"/>
    </link>
  </body>
</ncl>
