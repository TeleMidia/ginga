<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginSet">
        <connectorParam name="var"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
      <causalConnector id="onEndAttributionOrOnBeginSet">
        <connectorParam name="var"/>
        <compoundCondition operator="or">
          <simpleCondition role="onEndAttribution"/>
          <simpleCondition role="onBegin"/>
        </compoundCondition>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="time"/>
    <media id="time">
      <property name="background" value="red"/>
      <property name="bounds" value="25%,25%,50%,50%"/>
      <property name="dummy"/>
      <area id="a1" begin="1s"/>
      <area id="a2" begin="3s"/>
    </media>
    <link xconnector="onBeginSet">
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
    <link xconnector="onEndAttributionOrOnBeginSet">
      <bind role="onEndAttribution" component="time" interface="background"/>
      <bind role="onBegin" component="time" interface="a2"/>
      <bind role="set" component="time" interface="bounds">
        <bindParam name="var" value="0%,0%,100%,100%"/>
      </bind>
    </link>
    <link xconnector="onEndAttributionOrOnBeginSet">
      <bind role="onEndAttribution" component="time" interface="dummy"/>
      <bind role="onBegin" component="time" interface="a2"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="red"/>
      </bind>
    </link>
  </body>
</ncl>
