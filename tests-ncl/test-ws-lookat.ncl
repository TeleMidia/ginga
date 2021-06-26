<ncl>
  <head>
    <regionBase>
      <region id='reg1' left='0' top='25%' width='50%' height='50%'/>
      <region id='reg2' left='55%' top='40%' width='20%' height='20%'/>
      <region id='reg3' left='75%' top='40%' width='20%' height='20%'/>
    </regionBase>
    <descriptorBase>
      <descriptor id="desc1" region="reg1"/>
      <descriptor id="desc2" region="reg2" focusIndex="1" moveRight="2"/>
      <descriptor id="desc3" region="reg3" focusIndex="2" moveLeft="1"/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onLookAtStart">
        <simpleCondition role="onLookAt"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
      <causalConnector id="onLookAwayStart">
        <simpleCondition role="onLookAway"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
      <causalConnector id="onSelectionStartLookAt">
        <simpleCondition role="onSelection"/>
        <simpleAction role="startLookAt" eventType="lookat" actionType="start"/>
      </causalConnector>
      <causalConnector id="onSelectionStopLookAt">
        <simpleCondition role="onSelection"/>
        <simpleAction role="stopLookAt" eventType="lookat" actionType="stop"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="entry1" component="m1"/>
    <port id="entry2" component="m2"/>
    <port id="entry3" component="m3"/>
    <media id="m1" type="application/x-ginga-ncl360" descriptor="desc1">
      <property name="background" value="blue"/>
    </media>
    <media id="m2" descriptor="desc2">
      <property name="background" value="green"/>
    </media>
    <media id="m3" descriptor="desc3">
      <property name="background" value="red"/>
    </media>
    <link xconnector="onLookAtStart">
      <bind role="onLookAt" component="m1"/>
      <bind role="set" component="m1" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
    <link xconnector="onLookAwayStart">
      <bind role="onLookAway" component="m1"/>
      <bind role="set" component="m1" interface="background">
        <bindParam name="var" value="red"/>
      </bind>
    </link>
    <link xconnector="onSelectionStartLookAt">
      <bind role="onSelection" component="m2"/>
      <bind role="startLookAt" component="m1"/>
    </link>
    <link xconnector="onSelectionStopLookAt">
      <bind role="onSelection" component="m3"/>
      <bind role="stopLookAt" component="m1"/>
    </link>
  </body>
</ncl>