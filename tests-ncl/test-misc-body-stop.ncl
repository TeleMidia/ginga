<ncl>
  <head>
    <connectorBase>
      <causalConnector xconnector="onBeginStop">
        <simpleCondition role="onBegin"/>
        <simpleAction role="stop"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body id="self">
    <port id="start" component="time"/>
    <media id="time">
      <area id="a1" begin="2s"/>
      <property name="background" value="green"/>
      <property name="size" value="50%,50%"/>
    </media>
    <link>
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="stop" component="self"/>
    </link>
  </body>
</ncl>
