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

      <causalConnector id="onBeginStartStop">
         <simpleCondition role="onBegin" />
         <compoundAction>
         <simpleAction role="start" />
         <simpleAction role="stop" />
         </compoundAction>
      </causalConnector>

       <causalConnector id="onEndStopStart">
         <simpleCondition role="onEnd" />
         <compoundAction>
         <simpleAction role="stop" />
         <simpleAction role="start" />
         </compoundAction>
      </causalConnector>

    </connectorBase>
  </head>
  <body>
     <port id='start' component='img1'/>
     <media id='img1' src='samples/gnu.png' descriptor='desc' >
         <area id="segText" end="2s" />
     </media>
    
     <media id='img2' src='samples/vector.svg' descriptor='desc2' >
        <area id="segText" begin="2s" />
     </media>
     
    <link xconnector="onEndStopStart">
        <bind role="onEnd" component="img1" interface="segText" />
        <bind role="stop" component="img1"  />
        <bind role="start" component="img2"  />
     </link>

    <link xconnector="onBeginStartStop">
        <bind role="onBegin" component="img2" interface="segText" />
        <bind role="start" component="img1"  />
        <bind role="stop" component="img2"  />
     </link>     
   
  </body>
</ncl>
