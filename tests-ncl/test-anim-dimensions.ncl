<ncl>
  <head>
    <regionBase>
      <region id="reg" left="37.5%" top="37.5%" width="25%" height="25%"/>
    </regionBase>
    <descriptorBase>
      <descriptor id="desc" region="reg"/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onEndSet">
        <connectorParam name="var"/>
        <connectorParam name="delay"/>
        <connectorParam name="duration"/>
        <simpleCondition role="onEnd"/>
        <simpleAction role="set" value="$var"
                      delay="$delay" duration="$duration"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start" component="vid"/>
    <media id="vid" src="samples/clock.ogv" descriptor="desc">
      <area id="a1" end="1s"/>
      <area id="a2" end="5s"/>
      <property name="bounds"/>
    </media>
    <link xconnector="onEndSet">
      <bind role="onEnd" component="vid" interface="a1"/>
      <bind role="set" component="vid" interface="bounds">
        <bindParam name="var" value="0,0,800,600"/>
        <bindParam name="duration" value="2s"/>
      </bind>
    </link>
    <link xconnector="onEndSet">
      <bind role="onEnd" component="vid" interface="a2"/>
      <bind role="set" component="vid" interface="bounds">
        <bindParam name="var" value="0,0,100,100"/>
        <bindParam name="duration" value="2s"/>
      </bind>
    </link>
  </body>
</ncl>
