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
     <port id='start' component='img1'/>
     <media id='img1' src='samples/gnu.png' descriptor='desc' >
         <area id="segText" begin="0.5s" />
         <area id="segText1" begin="1s" />
     </media>
    
     <media id='img2' src='samples/vector.svg' descriptor='desc2' >
        <area id="segText" end="0.3s" />
        <area id="segText3" end="0.6s" />
        <area id="segText4" end="0.9s" />
        <area id="segText5" end="1.2s" />
        <area id="segText6" end="1.5s" />
        <area id="segText7" end="1.8s" />
     </media>
     
    <link xconnector="onBeginStart">
        <bind role="onBegin" component="img1" interface="segText" />
        <bind role="start" component="img2"  />
     </link>
   
     <link xconnector="onEndStop">
        <bind role="onEnd" component="img2" interface="segText" />
        <bind role="stop" component="img1"  />
     </link>

      <link xconnector="onEndStart">
        <bind role="onEnd" component="img2" interface="segText3" />
        <bind role="start" component="img1"  />
     </link>

     <link xconnector="onEndStop">
        <bind role="onEnd" component="img2" interface="segText4" />
        <bind role="stop" component="img1"  />
     </link>

      <link xconnector="onEndStart">
        <bind role="onEnd" component="img2" interface="segText5" />
        <bind role="start" component="img1"  />
     </link>

     <link xconnector="onEndStop">
        <bind role="onEnd" component="img2" interface="segText6" />
        <bind role="stop" component="img1"  />
     </link>

      <link xconnector="onEndStart">
        <bind role="onEnd" component="img2" interface="segText7" />
        <bind role="start" component="img1"  />
     </link>

  </body>
</ncl>
