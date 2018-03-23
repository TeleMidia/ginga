<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onKeySelSetStopStart">
        <connectorParam name="key"/>
        <connectorParam name="var"/>
        <simpleCondition role="onSelection" key="$key"/>
        <compoundAction operator="seq">
          <simpleAction role="set" value="$var"/>
          <simpleAction role="stop"/>
          <simpleAction role="start"/>
        </compoundAction>
      </causalConnector>
    </connectorBase>
    <ruleBase>
      <rule id="rColorRed" var="color" value="red" comparator="eq"/>
      <rule id="rColorGreen" var="color" value="green" comparator="eq"/>
      <rule id="rColorBlue" var="color" value="blue" comparator="eq"/>
      <rule id="rColorYellow" var="color" value="yellow" comparator="eq"/>
    </ruleBase>
  </head>
  <body>
    <port id="pSwitch" component="switch"/>
    <port id="pTimer" component="time"/>
    <media id="time"/>
    <media id="settings" type="application/x-ginga-settings">
      <property name="color" value="green"/>
    </media>
    <switch id="switch">
      <bindRule constituent="mRed" rule="rColorRed"/>
      <bindRule constituent="mGreen" rule="rColorGreen"/>
      <bindRule constituent="mBlue" rule="rColorBlue"/>
      <bindRule constituent="mYellow" rule="rColorYellow"/>
      <media id="mRed">
        <property name="background" value="red"/>
      </media>
      <media id="mGreen">
        <property name="background" value="green"/>
      </media>
      <media id="mBlue">
        <property name="background" value="blue"/>
      </media>
      <media id="mYellow">
        <property name="background" value="yellow"/>
      </media>
    </switch>
    <link xconnector="onKeySelSetStopStart">
      <bind role="onSelection" component="time">
        <bindParam name="key" value="RED"/>
      </bind>
      <bind role="set" component="settings" interface="color">
        <bindParam name="var" value="red"/>
      </bind>
      <bind role="stop" component="switch"/>
      <bind role="start" component="switch"/>
    </link>
    <link xconnector="onKeySelSetStopStart">
      <bind role="onSelection" component="time">
        <bindParam name="key" value="GREEN"/>
      </bind>
      <bind role="set" component="settings" interface="color">
        <bindParam name="var" value="green"/>
      </bind>
      <bind role="stop" component="switch"/>
      <bind role="start" component="switch"/>
    </link>
    <link xconnector="onKeySelSetStopStart">
      <bind role="onSelection" component="time">
        <bindParam name="key" value="BLUE"/>
      </bind>
      <bind role="set" component="settings" interface="color">
        <bindParam name="var" value="blue"/>
      </bind>
      <bind role="stop" component="switch"/>
      <bind role="start" component="switch"/>
    </link>
    <link xconnector="onKeySelSetStopStart">
      <bind role="onSelection" component="time">
        <bindParam name="key" value="YELLOW"/>
      </bind>
      <bind role="set" component="settings" interface="color">
        <bindParam name="var" value="yellow"/>
      </bind>
      <bind role="stop" component="switch"/>
      <bind role="start" component="switch"/>
    </link>
  </body>
</ncl>
