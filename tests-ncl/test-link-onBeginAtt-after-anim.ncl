<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginSet">
        <connectorParam name="var"/>
        <connectorParam name="duration"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="set" value="$var" duration="$duration"/>
      </causalConnector>
      <causalConnector id="onBeginAttributionSet">
        <connectorParam name="var"/>
        <simpleCondition role="onBeginAttribution"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="time"/>
    <media id="time">
      <property name="background" value="red"/>
      <property name="bounds" value="25%,25%,50%,50%"/>
      <area id="a1" begin="1s"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="green"/>
        <bindParam name="duration" value="2s"/>
      </bind>
    </link>
    <link xconnector="onBeginAttributionSet">
      <bind role="onBeginAttribution" component="time" interface="background"/>
      <bind role="set" component="time" interface="bounds">
        <bindParam name="var" value="0%,0%,100%,100%"/>
      </bind>
    </link>
  </body>
</ncl>
