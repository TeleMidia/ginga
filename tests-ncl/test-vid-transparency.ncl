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
        <simpleCondition role="onBegin"/>
        <simpleAction role="set" value="$var" max="unbounded"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="video"/>
    <media id="video" src="samples/clock.ogv" descriptor="desc">
      <area id="a1" begin="1s"/>
      <area id="a2" begin="2s"/>
      <area id="a3" begin="3s"/>
      <area id="a4" begin="4s"/>
      <area id="a5" begin="5s"/>
      <property name="transparency"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="video" interface="a1"/>
      <bind role="set" component="video" interface="transparency">
        <bindParam name="var" value="50%"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="video" interface="a2"/>
      <bind role="set" component="video" interface="transparency">
        <bindParam name="var" value="75%"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="video" interface="a3"/>
      <bind role="set" component="video" interface="transparency">
        <bindParam name="var" value="255"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="video" interface="a4"/>
      <bind role="set" component="video" interface="transparency">
        <bindParam name="var" value="127"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="video" interface="a5"/>
      <bind role="set" component="video" interface="transparency">
        <bindParam name="var" value="0%"/>
      </bind>
    </link>
  </body>
</ncl>
