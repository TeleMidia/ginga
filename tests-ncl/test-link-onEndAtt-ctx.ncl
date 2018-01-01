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
  <body>
    <port id="start1" component="time"/>
    <port id="start2" component="ctx"/>
    <media id="time">
      <property name="size" value="100%,100%"/>
      <property name="background" value="red"/>
      <area id="a1" begin="1s"/>
    </media>
    <context id="ctx">
      <property name="propName" value=""/>
      <port id="start0a" component="dummy"/>
      <media id="dummy"/>
    </context>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="set" component="ctx" interface="propName">
        <bindParam name="var" value="newPropValue"/>
      </bind>
    </link>
    <link xconnector="onEndAttributionSet">
      <bind role="onEndAttribution" component="ctx" interface="propName"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
  </body>
</ncl>
