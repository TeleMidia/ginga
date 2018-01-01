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
    <property name="propName" value="0"/>
    <port id="start" component="time"/>
    <media id="time">
      <property name="size" value="100%,100%"/>
      <property name="background" value="red"/>
      <area id="a1" begin="1s"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="set" component="self" interface="propName">
        <bindParam name="var" value="1"/>
      </bind>
    </link>
    <link xconnector="onEndAttributionSet">
      <bind role="onEndAttribution" component="self" interface="propName"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
  </body>
</ncl>
