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

       <causalConnector id="onBeginStop">
         <simpleCondition role="onBegin" />
         <simpleAction role="stop" />
      </causalConnector>

       <causalConnector id="onEndStop">
         <simpleCondition role="onEnd" />
         <simpleAction role="stop" />
      </causalConnector>

       <causalConnector id="onEndStart">
         <simpleCondition role="onEnd" />
         <simpleAction role="start" />
      </causalConnector>

      <causalConnector id="onKeySelectionStart">
        <connectorParam name="var"/>
        <simpleCondition role="onSelection" key="$var" />
        <simpleAction role="start" />
      </causalConnector>

    </connectorBase>
  </head>
  <body>
     <port id='start' component='vid1'/>
     <media id='vid1' src='samples/animGar.mp4' descriptor='desc' type='video/x-new-video-player' >
         <area id="segText" begin="3s" end="5s" />
     </media>

     <link xconnector="onBeginStop">
        <bind role="onBegin" component="vid1" interface="segText" />
        <bind role="stop" component="vid1"  />
     </link>

  </body>
</ncl>
