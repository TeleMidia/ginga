<ncl>
  <head>
    <connectorBase>
      <causalConnector xconnector="onBeginStop">
        <simpleCondition role="onBegin"/>
        <simpleAction role="stop"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="ctx"/>
    <context id="ctx">
      <port id="start0a" component="time0"/>
      <media id="time0">
        <area id="a0" begin="4s"/>
        <property name="bounds" value="10%,10%,100%,100%"/>
        <property name="background" value="red"/>
      </media>
    </context>
  </body>
</ncl>
