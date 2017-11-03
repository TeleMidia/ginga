<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginTestSet">
        <connectorParam name="var"/>
        <compoundCondition operator="and">
          <simpleCondition role="onBegin"/>
          <assessmentStatement comparator="eq">
            <attributeAssessment role="left"/>
            <attributeAssessment role="right"/>
          </assessmentStatement>
        </compoundCondition>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="time"/>
    <media id="time">
      <property name="background" value="red"/>
      <property name="x" value="1"/>
      <property name="y" value="2"/>
      <area id="a1" begin="1s"/>
      <area id="a2" begin="2s"/>
    </media>
    <link xconnector="onBeginTestSet">
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="left" component="time" interface="x"/>
      <bind role="right" component="time" interface="y"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
    <link xconnector="onBeginTestSet">
      <bind role="onBegin" component="time" interface="a2"/>
      <bind role="left" component="time" interface="x"/>
      <bind role="right" component="time" interface="x"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="blue"/>
      </bind>
    </link>
  </body>
</ncl>
