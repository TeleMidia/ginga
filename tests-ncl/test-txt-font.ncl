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
    <port id="start" component="text"/>
    <media id="text" src="samples/text.txt" descriptor="desc">
      <area id="a1" begin="1s"/>
      <area id="a2" begin="2s"/>
      <area id="a3" begin="3s"/>
      <area id="a4" begin="4s"/>
      <area id="a5" begin="5s"/>
      <property name="fontColor"/>
      <property name="fontBgColor"/>
      <property name="fontFamily"/>
      <property name="fontSize"/>
      <property name="fontStyle"/>
      <property name="fontVariant"/>
      <property name="fontWeight"/>
      <property name="horzAlign"/>
      <property name="vertAlign"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="text" interface="a1"/>
      <bind role="set" component="text" interface="fontColor">
        <bindParam name="var" value="navy"/>
      </bind>
      <bind role="set" component="text" interface="fontFamily">
        <bindParam name="var" value="serif"/>
      </bind>
      <bind role="set" component="text" interface="horzAlign">
        <bindParam name="var" value="right"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="text" interface="a2"/>
      <bind role="set" component="text" interface="fontSize">
        <bindParam name="var" value="30"/>
      </bind>
      <bind role="set" component="text" interface="fontFamily">
        <bindParam name="var" value="courier"/>
      </bind>
      <bind role="set" component="text" interface="horzAlign">
        <bindParam name="var" value="left"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="text" interface="a3"/>
      <bind role="set" component="text" interface="fontBgColor">
        <bindParam name="var" value="green"/>
      </bind>
      <bind role="set" component="text" interface="fontStyle">
        <bindParam name="var" value="italic"/>
      </bind>
      <bind role="set" component="text" interface="vertAlign">
        <bindParam name="var" value="middle"/>
      </bind>
      <bind role="set" component="text" interface="fontFamily">
        <bindParam name="var" value="times"/>
      </bind>
      <bind role="set" component="text" interface="fontSize">
        <bindParam name="var" value="40"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="text" interface="a4"/>
      <bind role="set" component="text" interface="fontWeight">
        <bindParam name="var" value="bold"/>
      </bind>
      <bind role="set" component="text" interface="fontSize">
        <bindParam name="var" value="12"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="text" interface="a5"/>
      <bind role="set" component="text" interface="fontStyle">
        <bindParam name="var" value="normal"/>
      </bind>
      <bind role="set" component="text" interface="fontVariant">
        <bindParam name="var" value="small-caps"/>
      </bind>
      <bind role="set" component="text" interface="horzAlign">
        <bindParam name="var" value="center"/>
      </bind>
      <bind role="set" component="text" interface="vertAlign">
        <bindParam name="var" value="bottom"/>
      </bind>
      <bind role="set" component="text" interface="fontSize">
        <bindParam name="var" value="20"/>
      </bind>
    </link>
  </body>
</ncl>
