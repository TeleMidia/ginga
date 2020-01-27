<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginStart">
        <simpleCondition role="onBegin"/>
        <simpleAction role="start" delay="1s"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="plua" component="lua"/>
    <media id="lua" src="../../nclua/examples/streambuf.lua">
      <property name="width" value="90%"/>
      <property name="height" value="10%"/>
      <property name="bottom" value="0%"/>
      <property name="left" value="3%"/>
      <property name="zIndex" value="2"/>
    </media>
    <media id="m" src="streambuf://b0">
      <property name="top" value="0%"/>
      <property name="left" value="0%"/>
      <property name="width" value="100%"/>
      <property name="height" value="100%"/>
      <property name="zIndex" value="1"/>
    </media>
    <link id="l1" xconnector="onBeginStart">
      <bind role="onBegin" component="lua"/>
      <bind role="start" component="m"/>
    </link>
  </body>
</ncl>
