<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onSelectionSet">
        <connectorParam name="key"/>
        <connectorParam name="var"/>
        <simpleCondition role="onSelection" key="$key"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="time"/>
    <media id="time">
      <property name="background" value="white"/>
      <property name="bounds" value="25%,25%,50%,50%"/>
    </media>
    <link xconnector="onSelectionSet">
      <bind role="onSelection" component="time">
        <bindParam name="key" value="RED"/>
      </bind>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="red"/>
      </bind>
    </link>
    <link xconnector="onSelectionSet">
      <bind role="onSelection" component="time">
        <bindParam name="key" value="GREEN"/>
      </bind>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
    <link xconnector="onSelectionSet">
      <bind role="onSelection" component="time">
        <bindParam name="key" value="BLUE"/>
      </bind>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="blue"/>
      </bind>
    </link>
    <link xconnector="onSelectionSet">
      <bind role="onSelection" component="time">
        <bindParam name="key" value="YELLOW"/>
      </bind>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="yellow"/>
      </bind>
    </link>
  </body>
</ncl>
