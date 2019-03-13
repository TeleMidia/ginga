<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onSelectionSet">
        <connectorParam name="var"/>
        <simpleCondition role="onSelection"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="ctx1"/>
    <media id="sets" type="application/x-ginga-settings">
      <property name="service.currentFocus" value="0" />
    </media>
    <port id="start0" component="m0"/>
    <media id="m0">
      <property name="background" value="red"/>
      <property name="zIndex" value="1"/>
    </media>
    <context id="ctx1">
      <port id="start1" component="m1"/>
      <port id="start2" component="m2"/>
      <media id="m1" src="samples/gnu.png">
        <property name="width" value="50%"/>
        <property name="height" value="50%"/>
        <property name="zIndex" value="2"/>
        <property name="focusIndex" value="0"/>
        <property name="moveUp" value="1"/>
        <property name="moveDown" value="1"/>
        <property name="moveLeft" value="1"/>
        <property name="moveRight" value="1"/>
      </media>
      <media id="m2" src="samples/gnu.png">
        <property name="left" value="50%"/>
        <property name="width" value="50%"/>
        <property name="height" value="50%"/>
        <property name="zIndex" value="2"/>
        <property name="focusIndex" value="1"/>
        <property name="moveUp" value="0"/>
        <property name="moveDown" value="0"/>
        <property name="moveLeft" value="0"/>
        <property name="moveRight" value="0"/>
      </media>
    </context>
    <link xconnector="onSelectionSet">
      <bind role="onSelection" component="ctx1" interface="start1"/>
      <bind role="set" component="m0" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
    <link xconnector="onSelectionSet">
      <bind role="onSelection"  component="ctx1" interface="start2"/>
      <bind role="set" component="m0" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
  </body>
</ncl>