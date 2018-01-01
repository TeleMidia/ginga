<ncl>
  <head>
    <connectorBase>
      <causalConnector id="onBeginPause">
        <connectorParam name="var"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="pause"/>
      </causalConnector>
      <!-- <causalConnector id="onBeginResume">
        <connectorParam name="var"/>
        <simpleCondition role="onBegin"/>
        <simpleAction role="resume"/>
      </causalConnector>
      <causalConnector id="onResumeSet">
        <connectorParam name="var"/>
        <simpleCondition role="onResumes"/>
        <simpleAction role="set" value="$var"/>
      </causalConnector> -->
    </connectorBase>
  </head>
  <body>
    <port id="start0" component="time0"/>
    <port id="start1" component="media0"/>
    <media id="time0">
      <area id="a1" begin="1s"/>
      <area id="a2" begin="2s"/>
    </media>
    <media id="media0">
      <property name="size" value="100%,100%"/>
      <property name="background" value="red"/>
    </media>
    <link xconnector="onBeginPause">
      <bind role="onBegin" component="time0" interface="a1"/>
      <bind role="pause" component="media0"/>
    </link>
    <!-- <link xconnector="onBeginResume">
      <bind role="onBegin" component="time0" interface="a2"/>
      <bind role="resume" component="media0"/>
    </link> -->
    <!-- <link xconnector="onResumeSet">
      <bind role="onResumes" component="media0"/>
      <bind role="set" component="media0" interface="background">
        <bindParam name="var" value="green"/>
      </bind>
    </link> -->
  </body>
</ncl>
