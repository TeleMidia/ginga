<ncl>
  <head>
    <regionBase>
      <region id="reg" width="100%" height="100%"/>
    </regionBase>
    <descriptorBase>
      <descriptor id="desc" region="reg"/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onBeginSet">
        <connectorParam name="var"/>
        <connectorParam name="delay"/>
        <connectorParam name="duration"/>
        <simpleCondition role="onBegin" key="$var"/>
        <simpleAction role="set" value="$var" delay="$delay"
                      duration="$duration"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="img"/>
    <media id="img" src="samples/gnu.png" descriptor="desc">
      <property name="transparency"/>
      <area id="a1" begin="1s"/>
      <area id="a2" begin="3s"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="img" interface="a1"/>
      <bind role="set" component="img" interface="transparency">
        <bindParam name="var" value="0%"/>
        <bindParam name="duration" value="1s"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="img" interface="a2"/>
      <bind role="set" component="img" interface="transparency">
        <bindParam name="var" value="100%"/>
        <bindParam name="duration" value="1s"/>
      </bind>
    </link>
  </body>
</ncl>
