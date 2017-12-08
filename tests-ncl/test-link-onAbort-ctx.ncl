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
  <context id="ctx">
    <property name="propName" value=""/>
    <port id="start0a" component="time0"/>
    <media id="time0">
      <property name="bounds" value="10%,10%,100%,100%"/>
      <property name="background" value="red"/>
    </media>
  </context>
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
