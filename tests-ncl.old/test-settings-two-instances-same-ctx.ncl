<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginSet">
        <connectorParam name="var"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start2" component="m1"/>
    <media id="settings1" type="application/x-ginga-settings">
      <property name="p1" />
    </media>
    <media id="settings2" type="application/x-ginga-settings">
      <property name="p1" />
    </media>
    <media id="m1">
      <property name="background" value="red"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="m1"/>
      <bind role="set" component="settings2" interface="p1">
        <bindParam name="var" value="1"/>
      </bind>
      <bind role="set" component="m1" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
  </body>
</ncl>