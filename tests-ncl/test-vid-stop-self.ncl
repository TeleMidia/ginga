<ncl>
  <head>
    <regionBase>
      <region id="reg" width="100%" height="100%"/>
    </regionBase>
    <descriptorBase>
      <descriptor id="desc" region="reg"/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onBeginStop">
        <simpleCondition role="onBegin"/>
        <simpleAction role="stop"/>
      </causalConnector>
      <causalConnector id="onEndStart">
        <simpleCondition role="onEnd"/>
        <simpleAction role="start"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="vid"/>
    <media id="vid" src="samples/clock.ogv" descriptor="desc">
      <area id="a1" begin="2s"/>
    </media>
    <media id="img" src="samples/gnu.png" descriptor="desc"/>
    <link xconnector="onBeginStop">
      <bind role="onBegin" component="vid" interface="a1"/>
      <bind role="stop" component="vid"/>
    </link>
    <link xconnector="onEndStart">
      <bind role="onEnd" component="vid"/>
      <bind role="start" component="img"/>
    </link>
  </body>
</ncl>
