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
        <simpleCondition role="onBegin"/>
        <simpleAction role="resume"/>
      </causalConnector>
      <causalConnector id="onResumeSet">
        <connectorParam name="var"/>
        <simpleCondition role="onResumes"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id="start0" component="time"/>
    <port id="start1" component="ctx"/>
    <media id="time">
      <property name="size" value="100%,100%"/>
      <property name="background" value="red"/>
      <area id="a1" begin="1s"/>
      <area id="a2" begin="2s"/>
    </media>
    <context id="ctx">
      <property name="propName" value=""/>
      <port id="start0a" component="dummy"/>
      <media id="dummy"/>
    </context>
    <link xconnector="onBeginPause">
      <bind role="onBegin" component="time" interface="a1"/>
      <bind role="pause" component="ctx"/>
    </link>
    <link xconnector="onBeginResume">
      <bind role="onBegin" component="time" interface="a2"/>
      <bind role="resume" component="ctx"/>
    </link>
    <link xconnector="onResumeSet">
      <bind role="onResumes" component="ctx"/>
      <bind role="set" component="time" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link>
  </body>
</ncl>
