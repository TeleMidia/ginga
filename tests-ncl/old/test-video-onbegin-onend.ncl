<ncl>
  <head>
    <regionBase>
      <region id='reg' width='50%' height='50%'/>
      <region id='reg2' top="50%" width='50%' height='50%'/>
    </regionBase>
    <descriptorBase>
      <descriptor id='desc' region='reg'/>
      <descriptor id='desc2' region='reg2'/>
    </descriptorBase>
    <connectorBase>
      <causalConnector id="onBeginStart">
        <simpleCondition role="onBegin" />
        <simpleAction role="start" />
      </causalConnector>
      <causalConnector id="onEndStop">
        <simpleCondition role="onEnd" />
        <simpleAction role="stop" />
      </causalConnector>
      <causalConnector id="onEndStart">
        <simpleCondition role="onEnd" />
        <simpleAction role="start" />
      </causalConnector>
    </connectorBase>
  </head>
  <body>
    <port id='start' component='vid1'/>
    <media id='vid1' src='../samples/small.mp4' descriptor='desc' />
    <media id='vid2' src='../samples/bunny.ogg' descriptor='desc' />
    <media id='img1' src='../samples/gnu.png' descriptor='desc2' />
    
    <link xconnector="onBeginStart">
        <bind role="onBegin" component="vid1"  />
        <bind role="start" component="img1"  />
    </link>
    <link xconnector="onEndStop">
        <bind role="onEnd" component="vid1"  />
        <bind role="stop" component="img1"  />
    </link>
    
    <link xconnector="onEndStart">
        <bind role="onEnd" component="vid1"  />
        <bind role="start" component="vid2"  />
    </link> 

  </body>
</ncl>
