<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginAbort">
        <connectorParam name="var"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="abort"/>
      </causalConnector>
      <causalConnector id="onAbortSet">
        <connectorParam name="var"/>
        <simpleCondition role="onAbort"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="time"/>
    <media id="time">
      <property name="background" value="red"/>
      <property name="bounds" value="0%,0%,100%,100%"/>
      <area id="a1" begin="1s"/>
    </media>
    <link xconnector="onBeginAbort">
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="abort" component="time"/>
    </link>
    <link xconnector="onAbortSet">
      <bind role="onAbort" component="time"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
  </body>
</ncl>
