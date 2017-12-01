<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginSet_var">
        <connectorParam name="var"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="ctx"/>
    <context id="ctx">
      <property name="propName" value=""/>
      <port id="start0a" component="time0"/>
      <media id="time0">
        <property name="bounds" value="10%,10%,100%,100%"/>
        <property name="background" value="red"/>
      </media>
    </context>
    <link xconnector="onBeginSet_var">
      <bind role="onBegin" component="ctx"/>
      <bind role="set" component="ctx" interface="propName">
        <bindParam name="var" value="newPropValue"/>
      </bind>
    </link>
  </body>
</ncl>
