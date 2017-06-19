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
    <media id="image" src="samples/text.txt" descriptor="desc">
      <area id="a1" begin="1s"/>
      <area id="a2" begin="2s"/>
      <area id="a3" begin="3s"/>
      <area id="a4" begin="4s"/>
      <area id="a5" begin="5s"/>
      <property name="width"/>
      <property name="height"/>
      <property name="top"/>
      <property name="left"/>
      <property name="bottom"/>
      <property name="right"/>
      <property name="bounds"/>
      <property name="location"/>
      <property name="size"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="image" interface="a1"/>
      <bind role="set" component="image" interface="width">
        <bindParam name="var" value="50%"/>
      </bind>
      <bind role="set" component="image" interface="height">
        <bindParam name="var" value="50%"/>
      </bind>
      <bind role="set" component="image" interface="top">
        <bindParam name="var" value="50%"/>
      </bind>
      <bind role="set" component="image" interface="left">
        <bindParam name="var" value="50%"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="image" interface="a2"/>
      <bind role="set" component="image" interface="bottom">
        <bindParam name="var" value="50%"/>
      </bind>
      <bind role="set" component="image" interface="right">
        <bindParam name="var" value="0%"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="image" interface="a3"/>
      <bind role="set" component="image" interface="bounds">
        <bindParam name="var" value="200,200px,200,200px"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="image" interface="a4"/>
      <bind role="set" component="image" interface="location">
        <bindParam name="var" value="50%,50%"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="image" interface="a5"/>
      <bind role="set" component="image" interface="size">
        <bindParam name="var" value="100%,100%"/>
      </bind>
    </link>
  </body>
</ncl>
