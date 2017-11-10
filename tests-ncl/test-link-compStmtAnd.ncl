<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginTestSet">
        <connectorParam name="var"/>
        <compoundCondition operator="and">
          <simpleCondition role="onBegin"/>
          <compoundStatement operator="and">
            <assessmentStatement comparator="eq">
              <attributeAssessment role="left1"/>
              <attributeAssessment role="right1"/>
            </assessmentStatement>
            <assessmentStatement comparator="eq">
              <attributeAssessment role="left2"/>
              <attributeAssessment role="right2"/>
            </assessmentStatement>
          </compoundStatement>
        </compoundCondition>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="time"/>
    <media id="time">
      <property name="background" value="red"/>
      <property name="x1" value="1"/>
      <property name="y1" value="2"/>
      <property name="x2" value="10"/>
      <property name="y2" value="20"/>
      <area id="a1" begin="1s"/>
      <area id="a2" begin="2s"/>
    </media>
    <link xconnector="onBeginTestSet">
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="left1" component="time" interface="x1"/>
      <bind role="right1" component="time" interface="y1"/>
      <bind role="left2" component="time" interface="x2"/>
      <bind role="right2" component="time" interface="y2"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
    <link xconnector="onBeginTestSet">
      <bind role="onBegin" component="time" interface="a2"/>
      <bind role="left1" component="time" interface="x1"/>
      <bind role="right1" component="time" interface="x1"/>
      <bind role="left2" component="time" interface="x2"/>
      <bind role="right2" component="time" interface="x2"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="blue"/>
      </bind>
    </link>
  </body>
</ncl>
