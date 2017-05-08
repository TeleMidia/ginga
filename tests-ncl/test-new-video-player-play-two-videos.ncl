<ncl>
  <head>
    <regionBase>
      <region id='reg' width='320' height='240'/>
      <region id='reg2' width='320' height='240' top='240'/> 
      <region id='reg3' width='320' height='240' left='320' top='240'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
      <descriptor id='desc2' region='reg2'/>
      <descriptor id='desc3' region='reg3'/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onBeginStart">
        <simpleCondition role="onBegin"/>
        <simpleAction role="start"/>
      </causalConnector>
      <causalConnector id="onBeginStop">
        <simpleCondition role="onBegin"/>
        <simpleAction role="stop"/>
      </causalConnector>
      <causalConnector id="onBeginPause">
        <simpleCondition role="onBegin"/>
        <simpleAction role="pause"/>
      </causalConnector>
      <causalConnector id="onBeginResume">
        <simpleCondition role="onBegin"/>
        <simpleAction role="resume"/>
      </causalConnector>

      <causalConnector id="onEndStart">
        <simpleCondition role="onEnd"/>
        <simpleAction role="start"/>
      </causalConnector>
      <causalConnector id="onEndStop">
        <simpleCondition role="onEnd"/>
        <simpleAction role="stop"/>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id='start' component='video'/>
    <!-- <port id='start' component='video2'/> -->
    
    <media id='video' src='samples/animGar.mp4' descriptor='desc' >
      <area id='area1' begin='5s'/>
      <area id='area4' begin='10s'/>
    </media>

    <media id='video2' src='samples/animGar.mp4' descriptor='desc2' >
      <area id='area2' begin='2s'/>
    </media>

    <media id='video3' src='samples/animGar.mp4' descriptor='desc3' >
      <area id='area3' begin='2s'/>
    </media>

    <link xconnector="onBeginStart">
      <bind role="onBegin" component="video" />
      <bind role="start" component="video2" />
    </link>
    
    <link xconnector="onEndStop">
      <bind role="onEnd" component="video2" />
      <bind role="stop" component="video" />
    </link>

  </body>
</ncl>
