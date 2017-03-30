<ncl>
  <head>
    <regionBase>
      <region id='reg' width='640' height='480'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onBeginStop">
        <simpleCondition role="onBegin"/>
        <simpleAction role="stop"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id='start' component='video'/>
    
    <media id='video' src='samples/bunny.ogg' descriptor='desc' type='video/x-new-video-player' >
      <area id='area1' begin='3s' />
    </media>
    <link xconnector="onBeginStop">
      <bind role="onBegin" component="video" interface="area1"/>
      <bind role="stop" component="video" />
    </link>
  </body>
</ncl>
