<ncl>
  <head>
    <regionBase>
      <region id='reg' width='640' height='480'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onBeginPause">
        <simpleCondition role="onBegin"/>
        <simpleAction role="pause"/>
      </causalConnector>
      <causalConnector id="onBeginResume">
        <simpleCondition role="onBegin"/>
        <simpleAction role="resume" delay="6s"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id='start' component='video'/>
    <media id='video' src='samples/bunny.ogg' descriptor='desc' >
      <area id='area1' begin='5s' />
    </media>
    <link xconnector="onBeginPause">
      <bind role="onBegin" component="video" interface="area1"/>
      <bind role="pause" component="video" />
    </link>
  </body>
</ncl>