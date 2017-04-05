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
         <area id="segText" begin="2s" />
         <area id="segText2" begin="2s" end="4s" />
     </media>
    
     <media id='img2' src='samples/vector.svg' descriptor='desc2' >
     </media>
     
    <link xconnector="onBeginStart">
        <bind role="onBegin" component="img1" interface="segText" />
        <bind role="start" component="img2"  />
     </link>
   

     <link xconnector="onEndStop">
        <bind role="onEnd" component="img1" interface="segText2" />
        <bind role="stop" component="img2"  />
     </link>

  </body>
</ncl>
