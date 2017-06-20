<ncl>
  <head>
    <regionBase>
      <region id='reg' width='640' height='480'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onKeySelectionPause">
        <connectorParam name="keyCode"/>
        <simpleCondition role="onSelection" key="$keyCode"/>
        <compoundAction operator="seq">
          <simpleAction role="pause" max="unbounded" qualifier="par"/>
        </compoundAction>
      </causalConnector>
      <causalConnector id="onKeySelectionResume">
        <connectorParam name="keyCode"/>
        <simpleCondition role="onSelection" key="$keyCode"/>
        <compoundAction operator="seq">
          <simpleAction role="resume" max="unbounded" qualifier="par"/>
        </compoundAction>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id='start' component='video'/>
    <media id='video' src='samples/bunny.ogg' descriptor='desc' type='video/x-new-video-player'/>
    <link xconnector="onKeySelectionPause">
      <bind role='onSelection' component='video'>
        <bindParam name='keyCode' value='RED'/>        
      </bind>
      <bind role="pause" component="video" />
    </link>
    <link xconnector="onKeySelectionResume">
      <bind role='onSelection' component='video'>
        <bindParam name='keyCode' value='GREEN'/>
      </bind>
      <bind role="resume" component="video" />
    </link>
  </body>
</ncl>
