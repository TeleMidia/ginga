<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginSet">
        <connectorParam name="var"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="set" value="$var" max="unbounded"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="m"/>
    <media id="m" src="">
      <property name="bounds" value="0%,0%,100%,100%"/>
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
      <property name="background" value="red"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="m" interface="a1"/>
      <bind role="set" component="m" interface="width">
        <bindParam name="var" value="50%"/>
      </bind>
      <bind role="set" component="m" interface="height">
        <bindParam name="var" value="50%"/>
      </bind>
      <bind role="set" component="m" interface="top">
        <bindParam name="var" value="50%"/>
      </bind>
      <bind role="set" component="m" interface="left">
        <bindParam name="var" value="50%"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="m" interface="a2"/>
      <bind role="set" component="m" interface="bottom">
        <bindParam name="var" value="50%"/>
      </bind>
      <bind role="set" component="m" interface="right">
        <bindParam name="var" value="0%"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="m" interface="a3"/>
      <bind role="set" component="m" interface="bounds">
        <bindParam name="var" value="200,200px,200,200px"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="m" interface="a4"/>
      <bind role="set" component="m" interface="location">
        <bindParam name="var" value="50%,50%"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="m" interface="a5"/>
      <bind role="set" component="m" interface="size">
        <bindParam name="var" value="100%,100%"/>
      </bind>
    </link>
  </body>
</ncl>
