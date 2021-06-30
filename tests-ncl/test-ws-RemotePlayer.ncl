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
      <causalConnector id="onSelectionStart">
        <simpleCondition role="onSelection"/>
        <simpleAction role="start" />
      </causalConnector>
      <causalConnector id="onSelectionStop">
        <simpleCondition role="onSelection"/>
        <simpleAction role="stop" />
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="entry1" component="hidden"/>
    <port id="entry2" component="m2"/>
    <port id="entry3" component="m3"/>
    <media id="hidden"/>
    <media id="m1" descriptor="desc1" type="application/x-ncl360"/>
    <media id="m2" descriptor="desc2">
      <property name="background" value="green"/>
    </media>
    <media id="m3" descriptor="desc3">
      <property name="background" value="red"/>
    </media>
    <link xconnector="onSelectionStart">
      <bind role="onSelection" component="m2"/>
      <bind role="start" component="m1"/>
    </link>
    <link xconnector="onSelectionStop">
      <bind role="onSelection" component="m3"/>
      <bind role="stop" component="m1"/>
    </link>
  </body>
</ncl>