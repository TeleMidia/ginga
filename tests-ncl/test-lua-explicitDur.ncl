<ncl>
  <head>
    <regionBase>
      <region id="reg1" top="0%" left="0%" width="50%" height="50%"/>
      <region id="reg2" top="0%" left="50%" width="50%" height="50%"/>
      <region id="reg3" bottom="0%" right="0%" width="50%" height="50%"/>
      <region id="reg4" bottom="0%" right="50%" width="50%" height="50%"/>
    </regionBase>
    <descriptorBase>
      <descriptor id="desc1" region="reg1"/>
      <descriptor id="desc2" region="reg2"/>
      <descriptor id="desc3" region="reg3"/>
      <descriptor id="desc4" region="reg4"/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onEndStart">
        <simpleCondition role="onEnd"/>
        <simpleAction role="start"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start1" component="lua0"/>
    <port id="start2" component="lua1"/>
    <media id="lua0" src="samples/fps.lua" descriptor="desc1">
      <property name="explicitDur" value="1s"/>
      <property name="background" value="red"/>
    </media>
    <media id="lua1" src="samples/fps.lua" descriptor="desc2">
      <property name="explicitDur" value="1s"/>
      <property name="background" value="green"/>
    </media>
    <link xconnector="onEndStart">
      <bind role="onEnd" component="lua1"/>
      <bind role="start" component="lua0"/>
      <bind role="start" component="lua1"/>
    </link>
    <port id="startctx" component="ctx"/>
    <context id="ctx">
      <port id="start3" component="lua2"/>
      <port id="start4" component="lua3"/>
      <media id="lua2" src="samples/fps.lua" descriptor="desc3">
        <property name="explicitDur" value="3s"/>
        <property name="background" value="blue"/>
      </media>
      <media id="lua3" src="samples/fps.lua" descriptor="desc4">
        <property name="explicitDur" value="2s"/>
      </media>
    </context>
  </body>
</ncl>
