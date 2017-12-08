<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginSet">
        <connectorParam name="var"/>
        <connectorParam name="duration"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="set" value="$var" duration="$duration"/>
      </causalConnector>
      <causalConnector id="onEndAttributionSet">
        <connectorParam name="var"/>
        <simpleCondition role="onEndAttribution"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body id="self">
    <property name="var" value="0"/>
    <port id="start" component="dummy"/>
    <media id="dummy">
      <property name="size" value="100%,100%"/>
      <property name="background" value="red"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="dummy"/>
      <bind role="set" component="self" interface="var">
        <bindParam name="var" value="1"/>
      </bind>
    </link>
    <link xconnector="onEndAttributionSet">
      <bind role="onEndAttribution" component="self" interface="var"/>
      <bind role="set" component="dummy" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
  </body>
</ncl>
