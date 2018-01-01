<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginPause">
        <connectorParam name="var"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="pause"/>
      </causalConnector>
      <causalConnector id="onBeginResume">
        <connectorParam name="var"/>
        <connectorParam name="delay"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="resume" delay="$delay"/>
      </causalConnector>
      <causalConnector id="onResumeSet">
        <connectorParam name="var"/>
        <simpleCondition role="onResumes"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body id="self">
    <port id="start0" component="time"/>
    <media id="time">
      <property name="size" value="100%,100%"/>
      <property name="background" value="red"/>
      <area id="a1" begin="1s"/>
    </media>
    <link xconnector="onBeginPause">
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="pause" component="self"/>
    </link>
    <link xconnector="onBeginResume">
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="resume" component="self">
        <bindParam name="delay" value="1s"/>
      </bind>
    </link>
    <link xconnector="onResumeSet">
      <bind role="onResumes" component="self"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
  </body>
</ncl>
