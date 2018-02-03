<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginStartDelay2s">
        <simpleCondition role="onBegin"/>
        <simpleAction role="start" delay="2s"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="time"/>
    <media id="time">
      <property name="background" value="red"/>
      <property name="bounds" value="25%,25%,50%,50%"/>
    </media>
    <media id="img" src="samples/gnu.png">
      <property name="bounds" value="0%,0%,50%,50%"/>
      <property name="explicitDur" value="5s"/>
      <property name="zIndex" value="1"/>
    </media>
    <link xconnector="onBeginStartDelay2s">
      <bind role="onBegin" component="time"/>
      <bind role="start" component="img"/>
    </link>
  </body>
</ncl>
