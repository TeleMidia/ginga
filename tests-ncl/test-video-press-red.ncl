<ncl>
  <head>
    <regionBase>
      <region id='reg' width='640' height='480'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onKeySelectionStop">
        <connectorParam name="keyCode"/>
        <simpleCondition role="onSelection" key="$keyCode"/>
        <compoundAction operator="seq">
          <simpleAction role="stop" max="unbounded" qualifier="par"/>
        </compoundAction>
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id='start' component='video'/>
    <media id='video' src='samples/bunny.ogg' descriptor='desc' type='video/x-new-video-player'/>
    <link xconnector="onKeySelectionStop">
      <bind role='onSelection' component='video'>
        <bindParam name='keyCode' value='RED'/>        
      </bind>
      <bind role="stop" component="video" />
    </link>
  </body>
</ncl>
