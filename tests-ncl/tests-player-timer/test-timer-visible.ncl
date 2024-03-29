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
    <port id="start0" component="m0"/>
    <port id="start1" component="m1"/>
    <port id="start2" component="m2"/>
    <port id="start3" component="m3"/>
    <media id="m0">
      <property name="visible"/>
      <property name="background" value="green"/>
      <property name="top" value="25%"/>
      <property name="left" value="25%"/>
      <property name="width" value="50%"/>
      <property name="height" value="50%"/>
      <property name="transparency" value="25%"/>
      <property name="zIndex" value="-1"/>
      <area id="a1" begin="1s"/>
      <area id="a2" begin="2s"/>
      <area id="a3" begin="3s"/>
      <area id="a4" begin="4s"/>
    </media>
    <media id="m1" src="">
      <property name="visible"/>
      <property name="width" value="50%"/>
      <property name="height" value="50%"/>
      <property name="top" value="25%"/>
      <property name="left" value="25%"/>
      <property name="zIndex" value="1"/>
    </media>
    <media id="m2" src="../samples/clock.ogv">
      <property name="visible"/>
      <property name="width" value="25%"/>
      <property name="height" value="25%"/>
      <property name="top" value="25%"/>
      <property name="left" value="25%"/>
      <property name="zIndex" value="2"/>
    </media>
    <media id="m3" src="../samples/text.txt">
      <property name="visible"/>
      <property name="width" value="25%"/>
      <property name="height" value="25%"/>
      <property name="top" value="50%"/>
      <property name="left" value="50%"/>
      <property name="zIndex" value="0"/>
      <property name="textAlign" value="right"/>
      <property name="fontColor" value="#ff0000"/>
      <property name="fontWeight" value="bold"/>
      <property name="fontFamily" value="helvetica"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="m0" interface="a1"/>
      <bind role="set" component="m2" interface="visible">
        <bindParam name="var" value="false"/>
      </bind>
      <bind role="set" component="m3" interface="visible">
        <bindParam name="var" value="true"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="m0" interface="a2"/>
      <bind role="set" component="m2" interface="visible">
        <bindParam name="var" value="true"/>
      </bind>
      <bind role="set" component="m3" interface="visible">
        <bindParam name="var" value="false"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="m0" interface="a3"/>
      <bind role="set" component="m2" interface="visible">
        <bindParam name="var" value="false"/>
      </bind>
      <bind role="set" component="m3" interface="visible">
        <bindParam name="var" value="true"/>
      </bind>
    </link>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="m0" interface="a4"/>
      <bind role="set" component="m2" interface="visible">
        <bindParam name="var" value="true"/>
      </bind>
      <bind role="set" component="m3" interface="visible">
        <bindParam name="var" value="true"/>
      </bind>
    </link>
  </body>
</ncl>
