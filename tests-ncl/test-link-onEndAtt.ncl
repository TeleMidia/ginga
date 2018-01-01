<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginSet">
        <connectorParam name="var"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
      <causalConnector id="onEndAttributionSet">
        <connectorParam name="var"/>
        <simpleCondition role="onEndAttribution"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="time"/>
    <media id="time">
      <property name="propName" value=""/>
      <property name="background" value="red"/>
      <property name="size" value="100%,100%"/>
      <area id="a1" begin="1s"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
    <link xconnector="onEndAttributionSet">
      <bind role="onEndAttribution" component="time" interface="propName"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
  </body>
</ncl>
