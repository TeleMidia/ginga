<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onEndStart">
        <simpleCondition role="onEnd"/>
        <simpleAction role="start"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start1" component="m0"/>
    <port id="start2" component="m1"/>
    <media id="m0" src="../samples/arcade.mp3">
      <property name="explicitDur" value="1s"/>
      <property name="background" value="red"/>
      <property name="bounds" value="0%,%0,50%,50%"/>
    </media>
    <media id="m1" src="../samples/arcade.mp3">
      <property name="explicitDur" value="2s"/>
      <property name="background" value="green"/>
      <property name="bounds" value="50%,50%,50%,50%"/>
    </media>
    <link xconnector="onEndStart">
      <bind role="onEnd" component="m1"/>
      <bind role="start" component="m0"/>
      <bind role="start" component="m1"/>
    </link>
  </body>
</ncl>
