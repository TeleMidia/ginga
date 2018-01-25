<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginStop">
        <simpleCondition role="onBegin"/>
        <simpleAction role="stop" />
      </causalConnector>
      <causalConnector id="onEndStop">
        <simpleCondition role="onEnd"/>
        <simpleAction role="stop" />
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="p1" component="ctx1"/>
    <context id="ctx1">
      <port id="p2" component="m1"/>
      <port id="p3" component="ctx2"/>
      <media id="m1">
        <area id="a1" begin="5s"/>
      </media>
      <context id="ctx2">
        <port id="p4" component="m2"/>
        <media id="m2" src="samples/bunny.ogg">
          <area id="a2" begin="3s"/>
        </media>
        <link xconnector="onBeginStop">
          <bind role="onBegin" component="m2" interface="a2"/>
          <bind role="stop" component="m2"/>
        </link>
      </context>
      <link xconnector="onEndStop">
        <bind role="onEnd" component="ctx2"/>
        <bind role="stop" component="m1"/>
      </link>
    </context>
  </body>
</ncl>