<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginStopStart">
        <simpleCondition role="onBegin"/>
        <compoundAction operator="seq">
          <simpleAction role="stop"/>
          <simpleAction role="start"/>
        </compoundAction>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="img"/>
    <media id="img" src="samples/gnu.png">
      <property name="bounds" value="25%,25%,50%,50%"/>
    </media>
    <link xconnector="onBeginStopStart">
      <bind role="onBegin" component="img"/>
      <bind role="stop" component="img"/>
      <bind role="start" component="img"/>
    </link>
  </body>
</ncl>
