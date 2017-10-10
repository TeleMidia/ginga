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
    <port id="start" component="time"/>
    <media id="time">
      <property name="background" value="red"/>
      <property name="bounds" value="25%,25%,50%,50%"/>
      <area id="a1" begin="1s"/>
      <area id="a2" begin="2s"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
  </body>
</ncl>
