<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onEndStart">
        <simpleCondition role="onEnd"/>
        <simpleAction role="start"/>
      </causalConnector>
    </connectorBase>
    <ruleBase>
      <rule id="rColorRed" var="color" value="any" comparator="eq"/>
      <rule id="rColorGreen" var="color" value="any" comparator="eq"/>
      <rule id="rColorBlue" var="color" value="any" comparator="eq"/>
      <rule id="rColorYellow" var="color" value="any" comparator="eq"/>
    </ruleBase>
  </head>
  <body>
    <port id="pSwitch" component="switch" interface="pRed"/>
    <media id="settings" type="application/x-ginga-settings">
      <property name="color" value="any"/>
    </media>
    <switch id="switch">
      <switchPort id="pRed">
        <mapping component="mRed"/>
      </switchPort>
      <switchPort id="pGreen">
        <mapping component="mGreen"/>
      </switchPort>
      <switchPort id="pBlue">
        <mapping component="mBlue"/>
      </switchPort>
      <switchPort id="pYellow">
        <mapping component="mYellow"/>
      </switchPort>
      <bindRule constituent="mRed" rule="rColorRed"/>
      <bindRule constituent="mGreen" rule="rColorGreen"/>
      <bindRule constituent="mBlue" rule="rColorBlue"/>
      <bindRule constituent="mYellow" rule="rColorYellow"/>
      <media id="mRed">
        <property name="background" value="red"/>
        <property name="explicitDur" value="2s"/>
      </media>
      <media id="mGreen">
        <property name="background" value="green"/>
        <property name="explicitDur" value="2s"/>
      </media>
      <media id="mBlue">
        <property name="background" value="blue"/>
        <property name="explicitDur" value="2s"/>
      </media>
      <media id="mYellow">
        <property name="background" value="yellow"/>
        <property name="explicitDur" value="2s"/>
      </media>
    </switch>
    <link xconnector="onEndStart">
      <bind role="onEnd" component="switch" interface="pRed"/>
      <bind role="start" component="switch" interface="pGreen"/>
    </link>
    <link xconnector="onEndStart">
      <bind role="onEnd" component="switch" interface="pGreen"/>
      <bind role="start" component="switch" interface="pBlue"/>
    </link>
    <link xconnector="onEndStart">
      <bind role="onEnd" component="switch" interface="pBlue"/>
      <bind role="start" component="switch" interface="pYellow"/>
    </link>

  </body>
</ncl>
