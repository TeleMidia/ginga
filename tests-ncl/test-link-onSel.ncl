<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onSelectionSet">
        <simpleCondition role="onSelection"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start1" component="one"/>
    <port id="start2" component="two"/>
    <media id="one">
      <property name="focusIndex" value="1"/>
      <property name="moveDown" value="2"/>
      <property name="moveUp" value="2"/>
      <property name="background" value="red"/>
      <property name="bounds" value="0%,0%,50%,50%"/>
    </media>
    <media id="two">
      <property name="focusIndex" value="2"/>
      <property name="moveDown" value="1"/>
      <property name="moveUp" value="1"/>
      <property name="background" value="green"/>
      <property name="bounds" value="50%,50%,50%,50%"/>
    </media>
    <link xconnector="onSelectionSet">
      <bind role="onSelection" component="one"/>
      <bind role="set" component="one" interface="background">
        <bindParam name="var" value="red"/>
      </bind>
      <bind role="set" component="two" interface="background">
        <bindParam name="var" value="red"/>
      </bind>
    </link>
    <link xconnector="onSelectionSet">
      <bind role="onSelection" component="two"/>
      <bind role="set" component="two" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
      <bind role="set" component="one" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
  </body>
</ncl>
