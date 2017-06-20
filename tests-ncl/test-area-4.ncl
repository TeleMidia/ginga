<ncl>
  <head>
    <regionBase>
      <region id="reg1" width="50%" height="50%"/>
      <region id="reg2" bottom="0" right="0" width="50%" height="50%"/>
    </regionBase>
    <descriptorBase>
      <descriptor id="desc1" region="reg1"/>
      <descriptor id="desc2" region="reg2"/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onBeginStart">
        <simpleCondition role="onBegin"/>
        <simpleAction role="start"/>
      </causalConnector>
      <causalConnector id="onEndStop">
        <simpleCondition role="onEnd"/>
        <simpleAction role="stop"/>
      </causalConnector>
      <causalConnector id="onEndStart">
        <simpleCondition role="onEnd"/>
        <simpleAction role="start"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="img1"/>
    <media id="img1" src="samples/gnu.png" descriptor="desc1">
      <area id="seg" begin="0.5s"/>
      <area id="seg1" begin="1s"/>
    </media>
    <media id="img2" src="samples/vector.svg" descriptor="desc2">
      <area id="seg" end="0.3s"/>
      <area id="seg3" end="0.6s"/>
      <area id="seg4" end="0.9s"/>
      <area id="seg5" end="1.2s"/>
      <area id="seg6" end="1.5s"/>
      <area id="seg7" end="1.8s"/>
    </media>
    <link xconnector="onBeginStart">
      <bind role="onBegin" component="img1" interface="seg"/>
      <bind role="start" component="img2"/>
    </link>
    <link xconnector="onEndStop">
      <bind role="onEnd" component="img2" interface="seg"/>
      <bind role="stop" component="img1"/>
    </link>
    <link xconnector="onEndStart">
      <bind role="onEnd" component="img2" interface="seg3"/>
      <bind role="start" component="img1"/>
    </link>
    <link xconnector="onEndStop">
      <bind role="onEnd" component="img2" interface="seg4"/>
      <bind role="stop" component="img1"/>
    </link>
    <link xconnector="onEndStart">
      <bind role="onEnd" component="img2" interface="seg5"/>
      <bind role="start" component="img1"/>
    </link>
    <link xconnector="onEndStop">
      <bind role="onEnd" component="img2" interface="seg6"/>
      <bind role="stop" component="img1"/>
    </link>
    <link xconnector="onEndStart">
      <bind role="onEnd" component="img2" interface="seg7"/>
      <bind role="start" component="img1"/>
    </link>
  </body>
</ncl>
