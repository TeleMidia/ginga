<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginStop">
        <simpleCondition role="onBegin"/>
        <simpleAction role="stop" />
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="ctx-start1" component="m1"/>
    <media id="m1" src="samples/gnu.png">
      <area id="a1" begin="2s"/>
    </media>
    <link xconnector="onBeginStop">
      <bind role="onBegin" component="m1" interface="a1"/>
      <bind role="stop" component="m1"/>
    </link>
  </body>
</ncl>
