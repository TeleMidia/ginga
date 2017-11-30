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
      <causalConnector id="onBeginStartStop">
        <simpleCondition role="onBegin"/>
        <compoundAction>
          <simpleAction role="start"/>
          <simpleAction role="stop"/>
        </compoundAction>
      </causalConnector>
      <causalConnector id="onEndStopStart">
        <simpleCondition role="onEnd"/>
        <compoundAction>
          <simpleAction role="stop"/>
          <simpleAction role="start"/>
        </compoundAction>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="img1"/>
    <media id="img1" src="samples/gnu.png" descriptor="desc1">
      <area id="segText1" end=".25s"/>
    </media>
    <media id="img2" src="samples/vector.svg" descriptor="desc2">
      <area id="segText2" begin=".25s"/>
    </media>
    <link xconnector="onEndStopStart">
      <bind role="onEnd" component="img1" interface="segText1"/>
      <bind role="stop" component="img1"/>
      <bind role="start" component="img2"/>
    </link>
    <link xconnector="onBeginStartStop">
      <bind role="onBegin" component="img2" interface="segText2"/>
      <bind role="start" component="img1"/>
      <bind role="stop" component="img2"/>
    </link>
  </body>
</ncl>
