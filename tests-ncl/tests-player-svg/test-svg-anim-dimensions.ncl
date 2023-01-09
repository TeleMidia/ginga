<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginSet">
        <connectorParam name="var"/>
        <connectorParam name="duration"/>
        <simpleCondition role="onBegin" key="$var"/>
        <simpleAction role="set" value="$var" duration="$duration"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="m"/>
    <media id="m" src="../samples/vector.svg">
      <property name="bounds" value="0%,0%,100%,100%"/>
      <property name="width"/>
      <property name="height"/>
      <area id="a1" begin="1s"/>
      <area id="a2" begin="3s"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="m" interface="a1"/>
      <bind role="set" component="m" interface="width">
        <bindParam name="var" value="25%"/>
        <bindParam name="duration" value="1s"/>
      </bind>
      <bind role="set" component="m" interface="height">
        <bindParam name="var" value="25%"/>
        <bindParam name="duration" value="1s"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="m" interface="a2"/>
      <bind role="set" component="m" interface="width">
        <bindParam name="var" value="100%"/>
        <bindParam name="duration" value="1s"/>
      </bind>
      <bind role="set" component="m" interface="height">
        <bindParam name="var" value="100%"/>
        <bindParam name="duration" value="1s"/>
      </bind>
    </link>
  </body>
</ncl>
