<ncl>
  <head>
    <regionBase>
      <region id="reg1" width="50%" height="50%"/>
      <region id="reg2" bottom="0" right="0" width="50%" height="50%"/>
    </regionBase>
    <descriptorBase>
      <descriptor id="desc1" region="reg1"/>
      <descriptor id="desc2" region="reg2"/>
      <descriptor id="desc3">
        <descriptorParam name="bounds" value="0%,50%,50%,50%"/>
      </descriptor>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onBeginStart">
        <simpleCondition role="onBegin"/>
        <simpleAction role="start" max="unbounded"/>
      </causalConnector>
      <causalConnector id="onEndStop">
        <simpleCondition role="onEnd"/>
        <simpleAction role="stop" max="unbounded"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="lua"/>
    <media id="lua" src="samples/area.lua" descriptor="desc1">
      <area id="a1" label="la1"/>
      <property name="bounds" value="50%,0,50%,50%"/>
    </media>
    <media id="vid" src="samples/clock.ogv" descriptor="desc3">
      <property name="zIndex" value="0"/>
    </media>
    <media id="img" src="samples/vector.svg" descriptor="desc2">
      <property name="zIndex" value="1"/>
    </media>
    <link xconnector="onBeginStart">
      <bind role="onBegin" component="lua" interface="a1"/>
      <bind role="start" component="img"/>
      <bind role="start" component="vid"/>
    </link>
    <link xconnector="onEndStop">
      <bind role="onEnd" component="lua" interface="a1"/>
      <bind role="stop" component="img"/>
      <bind role="stop" component="vid"/>
    </link>
  </body>
</ncl>
