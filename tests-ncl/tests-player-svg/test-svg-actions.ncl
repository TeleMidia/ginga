<ncl>
  <head>
    <regionBase>
      <region id="regM1" left="0" top="0%" width="50%" height="50%"/>
      <region id="regM2" left="0" top="50%" width="50%" height="50%"/>
      <region id="reg1" left="65%" top="02%" width="20%" height="10%"/>
      <region id="reg2" left="65%" top="22%" width="20%" height="10%"/>
      <region id="reg3" left="65%" top="42%" width="20%" height="10%"/>
      <region id="reg4" left="65%" top="62%" width="20%" height="10%"/>
      <region id="reg5" left="65%" top="82%" width="20%" height="10%"/>
    </regionBase>
    <descriptorBase>
      <descriptor id="descM1" region="regM1"/>
      <descriptor id="descM2" region="regM2"/>
      <descriptor id="desc1" region="reg1" focusIndex="1" moveDown="2" moveUp="5"/>
      <descriptor id="desc2" region="reg2" focusIndex="2" moveDown="3" moveUp="1"/>
      <descriptor id="desc3" region="reg3" focusIndex="3" moveDown="4" moveUp="2"/>
      <descriptor id="desc4" region="reg4" focusIndex="4" moveDown="5" moveUp="3"/>
      <descriptor id="desc5" region="reg5" focusIndex="5" moveDown="1" moveUp="4"/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onSelectionStart">
        <simpleCondition role="onSelection"/>
        <compoundAction>
          <simpleAction role="start"/>
        </compoundAction>
      </causalConnector>
      <causalConnector id="onSelectionStop">
        <simpleCondition role="onSelection"/>
        <compoundAction>
          <simpleAction role="stop"/>
        </compoundAction>
      </causalConnector>
      <causalConnector id="onSelectionPause">
        <simpleCondition role="onSelection"/>
        <compoundAction>
          <simpleAction role="pause"/>
        </compoundAction>
      </causalConnector>
      <causalConnector id="onSelectionResume">
        <simpleCondition role="onSelection"/>
        <compoundAction>
          <simpleAction role="resume"/>
        </compoundAction>
      </causalConnector>
      <causalConnector id="onSelectionAbort">
        <simpleCondition role="onSelection"/>
        <compoundAction>
          <simpleAction role="abort"/>
        </compoundAction>
      </causalConnector>
      <causalConnector id="onEndStart">
        <simpleCondition role="onEnd"/>
        <simpleAction role="start"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start0a" component="video"/>
    <port id="start0b" component="ctx"/>

    <port id="start1" component="cmd1"/>
    <port id="start2" component="cmd2"/>
    <port id="start3" component="cmd3"/>
    <port id="start4" component="cmd4"/>
    <port id="start5" component="cmd5"/>
    <!-- main medias -->
    <media id="video" src="../samples/vector.svg" descriptor="descM1"/>
    <context id="ctx">
      <port id="startctx" component="videoctx"/>
      <media id="videoctx" src="../samples/vector.svg" descriptor="descM2"/>
    </context>

    <!-- controls -->
    <media id="cmd1" src="../samples/msg-start.txt" descriptor="desc1">
      <property name="fontColor" value="#E6F732" />
      <property name="backgroundColor" value="#000055" />
      <property name="horzAlign" value="center" />
      <property name="vertAlign" value="middle" />
    </media>
    <media id="cmd2" src="../samples/msg-stop.txt" descriptor="desc2">
      <property name="fontColor" value="#E6F732" />
      <property name="backgroundColor" value="#000055" />
      <property name="horzAlign" value="center" />
      <property name="vertAlign" value="middle" />
    </media>
    <media id="cmd3" src="../samples/msg-pause.txt" descriptor="desc3">
      <property name="fontColor" value="#E6F732" />
      <property name="backgroundColor" value="#000055" />
      <property name="horzAlign" value="center" />
      <property name="vertAlign" value="middle" />
    </media>
    <media id="cmd4" src="../samples/msg-resume.txt" descriptor="desc4">
      <property name="fontColor" value="#E6F732" />
      <property name="backgroundColor" value="#000055" />
      <property name="horzAlign" value="center" />
      <property name="vertAlign" value="middle" />
    </media>
    <media id="cmd5" src="../samples/msg-abort.txt" descriptor="desc5">
      <property name="fontColor" value="#E6F732" />
      <property name="backgroundColor" value="#000055" />
      <property name="horzAlign" value="center" />
      <property name="vertAlign" value="middle" />
    </media>
    <link xconnector="onSelectionStart">
      <bind role="onSelection" component="cmd1"/>
      <bind role="start" component="video"/>
      <bind role="start" component="ctx" interface="startctx"/>
    </link>
    <link xconnector="onSelectionStop">
      <bind role="onSelection" component="cmd2"/>
      <bind role="stop" component="video"/>
      <bind role="stop" component="ctx" interface="startctx"/>
    </link>
    <link xconnector="onSelectionPause">
      <bind role="onSelection" component="cmd3"/>
      <bind role="pause" component="video"/>
      <bind role="pause" component="ctx" interface="startctx"/>
    </link>
    <link xconnector="onSelectionResume">
      <bind role="onSelection" component="cmd4"/>
      <bind role="resume" component="video"/>
      <bind role="resume" component="ctx" interface="startctx"/>
    </link>
    <link xconnector="onSelectionAbort">
      <bind role="onSelection" component="cmd5"/>
      <bind role="abort" component="video"/>
      <bind role="abort" component="ctx" interface="startctx"/>
    </link>
  </body>
</ncl>