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
    <port id="start" component="image"/>
    <media id="image" src="samples/vector.svg" descriptor="desc">
      <area id="a1" begin="1s"/>
      <area id="a2" begin="2s"/>
      <area id="a3" begin="3s"/>
      <area id="a4" begin="4s"/>
      <area id="a5" begin="5s"/>
      <property name="backgroundColor"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="image" interface="a1"/>
      <bind role="set" component="image" interface="backgroundColor">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="image" interface="a2"/>
      <bind role="set" component="image" interface="backgroundColor">
        <bindParam name="var" value="#ff0000"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="image" interface="a3"/>
      <bind role="set" component="image" interface="backgroundColor">
        <bindParam name="var" value="rgb(127,33,255)"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="image" interface="a4"/>
      <bind role="set" component="image" interface="backgroundColor">
        <bindParam name="var" value="purple"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="image" interface="a5"/>
      <bind role="set" component="image" interface="backgroundColor">
        <bindParam name="var" value="black"/>
      </bind>
    </link>
  </body>
</ncl>
