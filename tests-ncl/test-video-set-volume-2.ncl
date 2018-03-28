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
    <port id="start" component="audio"/>
    <media id="audio" src="samples/arcade.mp3" descriptor="desc">
      <area id="a1" begin="3s"/>
      <area id="a2" begin="6s"/>
      <area id="a3" begin="9s"/>
      <area id="a4" begin="12s"/>
      <area id="a5" begin="15s"/>
      <area id="a6" begin="18s"/>
      <area id="a7" begin="21s"/>
      <property name="bounds" value="25%,25%,50%,50%"/>
      <property name="debug" value="true"/>
      <property name="soundLevel" value="50%"/>
      <property name="volume" value="50%"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="audio" interface="a1"/>
      <bind role="set" component="audio" interface="soundLevel">
        <bindParam name="var" value="50%"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="audio" interface="a2"/>
      <bind role="set" component="audio" interface="soundLevel">
        <bindParam name="var" value="150%"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="audio" interface="a3"/>
      <bind role="set" component="audio" interface="volume">
        <bindParam name="var" value="5.0"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="audio" interface="a4"/>
      <bind role="set" component="audio" interface="soundLevel">
        <bindParam name="var" value="20%"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="audio" interface="a5"/>
      <bind role="set" component="audio" interface="volume">
        <bindParam name="var" value="100%"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="audio" interface="a6"/>
      <bind role="set" component="audio" interface="volume">
        <bindParam name="var" value="0%"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="audio" interface="a7"/>
      <bind role="set" component="audio" interface="volume">
        <bindParam name="var" value="200%"/>
      </bind>
    </link>
  </body>
</ncl>