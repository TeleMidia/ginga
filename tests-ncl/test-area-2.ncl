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
    <port id="start" component="img1"/>
    <media id="img1" src="samples/gnu.png" descriptor="desc1">
      <area id="seg" end="2s"/>
      <area id="seg2" begin="4s"/>
    </media>
    <media id="img2" src="samples/vector.svg" descriptor="desc2">
    </media>
    <link xconnector="onEndStart">
      <bind role="onEnd" component="img1" interface="seg"/>
      <bind role="start" component="img2"/>
    </link>
    <link xconnector="onBeginStop">
      <bind role="onBegin" component="img1" interface="seg2"/>
      <bind role="stop" component="img2"/>
    </link>
  </body>
</ncl>
