<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginStart">
        <simpleCondition role="onBegin"/>
        <simpleAction role="start"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start0" component="m0"/>
    <port id="start1" component="ctx" interface="start2"/>
    <media id="m0" />
    <media id="m3">
      <property name="background" value="red"/>
    </media>
    <context id="ctx">
      <port id="start2" component="m1"/>
      <media id="m1" src="samples/gnu.png"/>
      <media id="m2">
        <property name="background" value="red"/>
      </media>
      <link xconnector="onBeginStart">
        <bind role="onBegin" component="m1"/>
        <bind role="start" component="m2"/>
      </link>
    </context>
    <link xconnector="onBeginStart">
      <bind role="onBegin" component="ctx" interface="start2"/>
      <bind role="start" component="m3"/>
    </link>
  </body>
</ncl>