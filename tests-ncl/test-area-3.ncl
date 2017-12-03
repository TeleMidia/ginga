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
    <port id="start" component="img1"/>
    <media id="img1" src="samples/gnu.png" descriptor="desc1">
      <area id="seg1" begin="2s"/>
    </media>
    <media id="img2" src="samples/vector.svg" descriptor="desc2">
      <area id="seg2" end="2s"/>
    </media>
    <media id="vid" src="samples/clock.ogv" descriptor="desc3"/>
    <media id="lua" src="samples/fps.lua">
      <property name="bounds" value="50%,0,50%,50%"/>
    </media>
    <link xconnector="onBeginStart">
      <bind role="onBegin" component="img1" interface="seg1"/>
      <bind role="start" component="img2"/>
      <bind role="start" component="vid"/>
      <bind role="start" component="lua"/>
    </link>
    <link xconnector="onEndStop">
      <bind role="onEnd" component="img2" interface="seg2"/>
      <bind role="stop" component="img1"/>
      <bind role="stop" component="vid"/>
      <bind role="stop" component="lua"/>
    </link>
  </body>
</ncl>
