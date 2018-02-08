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
    <port id="start1" component="m1"/>
    <port id="start2" component="m2"/>
    <media id="settings" type="application/x-ginga-settings">
      <property name="service.currentFocus" value="0" />
    </media>
    <media id="m1" src="samples/gnu.png">
      <property name="width" value="50%"/>
      <property name="height" value="50%"/>
      <property name="background" value="red"/>
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
      <property name="background" value="green"/>
      <property name="focusIndex" value="1"/>
      <property name="moveUp" value="0"/>
      <property name="moveDown" value="0"/>
      <property name="moveLeft" value="0"/>
      <property name="moveRight" value="0"/>
    </media>
    <link xconnector="onSelectionSet">
      <bind role="onSelection" component="m1"/>
      <bind role="set" component="settings" interface="service.currentFocus">
        <bindParam name="var" value="1"/>
      </bind>
    </link>
    <link xconnector="onSelectionSet">
      <bind role="onSelection" component="m2"/>
      <bind role="set" component="settings" interface="service.currentFocus">
        <bindParam name="var" value="0"/>
      </bind>
    </link>
  </body>
</ncl>